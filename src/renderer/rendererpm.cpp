#include "../renderer/rendererpm.h"
#include "../scene/texture.h"
#include "../sampler/lightsampler.h"
#include "../hierarchy/bvh.h"
#include <QTime>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include "../sampler/samplerstd.h"
#include "../hierarchy/kdtree.h"

vec3 RendererPM::trace(Config &config, const KDTree<Photon> &photon_map, Sampler &sampler, const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler_, BVH &bvh, const Envmap *env_map)
{
    auto [t, b1, b2, hit_obj] = intersect(orig, dir, triangles, bvh);
    if (hit_obj == nullptr)
    {
        if (env_map == nullptr)
        {
            return 0.0f;
        }
        else
        {
            float v = 1 - acos(dir[1]) / 3.14159;
            float u = atan2(dir[2], dir[0]) / 2 / 3.14159 + 0.5;
            return env_map->pixelUV(u, v);
        }
    }

    vec3 wo = -dir;
    vec3 normal = hit_obj->getNormal(b1, b2);

    if (hit_obj->mat->isEmission())
    {
        return hit_obj->mat->emission(wo, normal);
    }

    vec3 texcoords = hit_obj->getTexCoords(b1, b2);
    float u = texcoords[0], v = texcoords[1];

    if (normal.dot(dir) > 0 && hit_obj->mat->isTransmission() == false)
        return vec3(0.0f, 0.0f, 0.0f);

    vec3 hit_pos = orig + dir * t;
    vec3 result;

    auto photon_cmp = [&](const Photon &a, const Photon &b)
    {
        float dist_a = (a.pos - hit_pos).norm2();
        float dist_b = (b.pos - hit_pos).norm2();
        return dist_a < dist_b;
    };

    if (hit_obj->mat->isSpecular(wo, normal, wo, texcoords) == false)
    {
        int photon_limit = config.getValueInt("photon_k", 32);

        // Radiance Estimate: find nearest k photons and estimate the radiance
        // std::priority_queue<Photon, std::vector<Photon>, decltype(photon_cmp)> photon_queue(photon_cmp);
        // for (const auto &photon : photon_map)
        // {
        //     photon_queue.push(photon);
        //     if (photon_queue.size() > photon_limit)
        //     {
        //         photon_queue.pop();
        //     }
        // }

        auto photon_list = photon_map.kNN(hit_pos, photon_limit);

        if (photon_list.size())
        {
            float d2 = (photon_list[0]->pos - hit_pos).norm2();
            vec3 radiance;
            for (auto photon : photon_list)
            {
                vec3 wi = photon->dir;
                vec3 bxdf = hit_obj->mat->bxdf(wo, normal, wi, texcoords);
                float pdf = hit_obj->mat->pdf(wo, normal, wi);
                radiance += photon->power * bxdf;
            }
            return radiance / 3.14159 / d2;
        }
    }

    // Round Robin
    float prr = 0.8;
    if (sampler.random() > prr)
        return result;

    // sample bxdf
    vec3 wi = hit_obj->mat->sampleBxdf(sampler, wo, normal);
    float pdf = hit_obj->mat->pdf(wo, normal, wi);
    vec3 brdf = hit_obj->mat->bxdf(wo, normal, wi, texcoords);
    vec3 Li = trace(config, photon_map, sampler, hit_pos + wi * 1e-3, wi, triangles, light_sampler_, bvh, env_map);
    vec3 contri = Li * abs(wi.dot(normal)) * brdf / pdf / prr;
    result += contri;

    return result;
}

void RendererPM::render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img, Config &config, std::function<void(bool)> display_update_callback, std::atomic<int> &con_flag, std::function<void(float)> progress_report_callback, QMutex &framebuffer_mutex, const Envmap *env_map)
{
    int img_width = config.getValueInt("imgw", 1);
    int img_height = config.getValueInt("imgh", 1);
    int SPP = config.getValueInt("spp", 1);

    SamplerStd sampler;
    display_update_callback(false);
    framebuffer_mutex.lock();
    img = QImage(QSize(img_width, img_height), QImage::Format_RGB888);
    img.fill(Qt::black);
    framebuffer_mutex.unlock();

    QTime time;
    time.start();
    auto time_last = time.elapsed();

    std::cout << "Rendering... " << std::endl;

    std::atomic<int> progress_unit_counter = 0;

    auto requestProgressUpdate = [&]()
    {
        float progress = progress_unit_counter * 1.0f / img_height / img_width;
        progress_report_callback(progress * 100);
        if (time.elapsed() - time_last > 1000)
        {
            std::cout << std::fixed << std::setprecision(2) << "Rendering... " << progress * 100 << "%"
                      << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
            time_last = time.elapsed();
        }
    };

    std::cout << "Trace photons..." << std::endl;

    auto hemisphereSampler = [&](const vec3 &normal) -> vec3
    {
        float r2 = sampler.random() * 0.999f;
        float phi = sampler.random() * 3.14159 * 2;
        float r = sqrt(r2);
        float h = sqrt(1 - r2);

        vec3 ax0 = abs(normal[0]) < 0.8 ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f);
        vec3 ax1 = ax0.cross(normal).normalized();
        vec3 ax2 = normal.cross(ax1).normalized();
        vec3 wi = h * normal + r * cos(phi) * ax1 + r * sin(phi) * ax2;
        return wi;
    };

    std::vector<Photon> photon_list;

    int n_photons = config.getValueInt("n_photons", 100000);
#pragma omp parallel for
    for (int i = 0; i < n_photons; i++)
    {
        SamplerStd sampler;
        // emit photon
        auto [light_pos, light_dir, light_int] = light_sampler_.sampleAllLight(sampler);
        Photon photon;
        photon.pos = light_pos;
        photon.dir = light_dir;
        photon.power = 2 * 3.14159 * light_int / n_photons;

        // trace photon
        while (true)
        {
            auto orig = photon.pos, dir = photon.dir;
            auto [t, b1, b2, hit_obj] = intersect(orig, dir, triangles, bvh_);
            if (hit_obj == nullptr)
                break;
            vec3 wo = -dir;
            vec3 normal = hit_obj->getNormal(b1, b2);

            if (hit_obj->mat->isEmission())
            {
                break;
            }

            vec3 texcoords = hit_obj->getTexCoords(b1, b2);
            float u = texcoords[0], v = texcoords[1];
#pragma omp critical
            if (hit_obj->mat->isSpecular(wo, normal, wo, texcoords) == false)
                photon_list.push_back(photon);

            if (normal.dot(dir) > 0 && hit_obj->mat->isTransmission() == false)
                break;

            vec3 hit_pos = orig + dir * t;

            float prr = 0.8;
            if (sampler.random() > prr)
                break;

            vec3 wi = hit_obj->mat->sampleBxdf(sampler, wo, normal);
            vec3 brdf = hit_obj->mat->bxdf(wo, normal, wi, texcoords);
            float pdf = hit_obj->mat->pdf(wo, normal, wi);
            photon.pos = hit_pos + wi * 1e-3;
            photon.dir = wi;
            photon.power *= brdf * abs(wi.dot(normal)) / pdf / prr;
        }
    }

    KDTree<Photon> photon_map;
    std::vector<std::pair<vec3, Photon *>> photon_list_cvt;
    for (auto &i : photon_list)
    {
        photon_list_cvt.push_back({i.pos, &i});
    }
    photon_map.build(photon_list_cvt);

    int block_size = config.getValueInt("blocksize", 4);
    std::vector<std::pair<int, int>> task_queue;
    std::mutex task_mutex;

    std::atomic request_disp_update = 0;

    for (int y = 0; y < img_height; y += block_size)
    {
        for (int x = 0; x < img_width; x += block_size)
        {
            task_queue.push_back({x, y});
        }
    }

    std::random_shuffle(task_queue.begin(), task_queue.end());

    auto workerFunc = [&]()
    {
        while (true)
        {
            if (con_flag == 0)
            {
                break;
            }

            task_mutex.lock();
            if (task_queue.size() == 0)
            {
                task_mutex.unlock();
                break;
            }
            auto [x0, y0] = task_queue.back();
            task_queue.pop_back();
            task_mutex.unlock();

            for (int y = y0; y < img_height && y < y0 + block_size; y++)
            {
                for (int x = x0; x < img_width && x < x0 + block_size; x++)
                {
                    vec3 result;
                    for (int i = 0; i < SPP; i++)
                    {
                        vec3 ray_dir = camera.generateRay(x + sampler.random(), y + sampler.random(), img_width, img_height);
                        result += max(0.0f, trace(config, photon_map, sampler, camera.pos, ray_dir, triangles, light_sampler_, bvh_, env_map));
                    }
                    result /= SPP;
                    // Gamma correction
                    result = result.pow(1.0 / 2.2);
                    result *= 255.0f;
                    framebuffer_mutex.lock();
                    img.setPixel(x, y, qRgb(std::min(255.0f, std::max(0.0f, result[0])), std::min(255.0f, std::max(0.0f, result[1])), std::min(255.0f, std::max(0.0f, result[2]))));
                    framebuffer_mutex.unlock();

                    vec3 ray_dir = camera.generateRay(x + 0.5f, y + 0.5f, img_width, img_height);
                }
            }
            progress_unit_counter += std::min(img_height - y0, block_size) * std::min(img_width - x0, block_size);;
            requestProgressUpdate();
            request_disp_update = 1;
        }
    };

    int num_threads = config.getValueInt("parallel", 4);
    std::vector<std::thread> ths;
    for (int i = 0; i < num_threads; i++)
    {
        ths.push_back(std::thread(workerFunc));
    }

    while (progress_unit_counter < img_width * img_height)
    {
        if (request_disp_update == 1)
        {
            request_disp_update = 0;
            display_update_callback(false);
        }
        // Just delay
        QEventLoop loop;
        QTimer timer;
        timer.setInterval(100);
        timer.start();
        QObject::connect(&timer, &QTimer::timeout, [&]()
                         { loop.quit(); });
        loop.exec();
        if (con_flag == 0)
        {
            break;
        }
    }

    for (auto &i : ths)
    {
        i.join();
    }

    if (con_flag == 0)
    {
        std::cout << "Cancelled" << std::endl;
        return;
    }

    std::cout << std::fixed << std::setprecision(2) << "Rendering... " << 100.0 << "%"
              << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
    con_flag = 0;
    display_update_callback(true);
}
