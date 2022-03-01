#include "renderer.h"
#include "texture.h"
#include "lightsampler.h"
#include "bvh.h"
#include <QTime>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>

std::tuple<float, float, float, const Triangle *> Renderer::intersect(const vec3 &origin, const vec3 &dir, const std::vector<Triangle> &triangles, BVH &bvh)
{
    // float m_t = 1e18, m_b1 = 0, m_b2 = 0;
    // const Triangle *hit_obj = nullptr;
    // for (auto &triangle : triangles)
    // {
    //     auto [t, b1, b2] = triangle.intersection(origin, dir);
    //     if (t < m_t && t > 0 && b1 >= 0 && b2 >= 0 && b1 + b2 <= 1)
    //     {
    //         m_t = t;
    //         m_b1 = b1;
    //         m_b2 = b2;
    //         hit_obj = &triangle;
    //     }
    // }
    // return {m_t, m_b1, m_b2, hit_obj};

    return bvh.intersection(origin, dir);
}

vec3 Renderer::trace(const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler, BVH &bvh, bool light_source_visible, const Texture *env_map)
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
        if (light_source_visible)
            return hit_obj->mat->emission(wo, normal);
        else
            return vec3(0.0f, 0.0f, 0.0f);
    }

    vec3 texcoords = hit_obj->getTexCoords(b1, b2);
    float u = texcoords[0], v = texcoords[1];

    if (normal.dot(dir) > 0 && hit_obj->mat->isTransmission() == false)
        return vec3(0.0f, 0.0f, 0.0f);

    vec3 hit_pos = orig + dir * t;
    vec3 result;

    // sample the light
    bool is_light_sampled = false;
    if (hit_obj->mat->requireLightSampling(wo, normal))
    {
        is_light_sampled = true;
        const Triangle *light_obj = light_sampler.sampleLight();
        if (light_obj != nullptr)
        {
            auto [light_pos, light_bc1, light_bc2] = light_obj->sample();
            vec3 light_normal = light_obj->getNormal(light_bc1, light_bc2);
            vec3 light_uv = light_obj->getTexCoords(light_bc1, light_bc2);
            vec3 light_vec = light_pos - hit_pos;
            vec3 wl = light_vec.normalized();
            vec3 light_int = light_obj->mat->emission(wl, light_normal);
            float light_pdf = light_sampler.p();
            auto [light_ray_t, light_ray_b1, light_ray_b2, light_ray_hit_obj] = intersect(hit_pos + wl * 1e-3, wl, triangles, bvh);
            if (light_ray_t + 2e-3 > light_vec.norm())
            {
                vec3 brdf_ = hit_obj->mat->bxdf(wo, normal, wl, texcoords);
                vec3 Ll = light_int / light_vec.norm2() * std::max(0.0f, light_normal.dot(-wl)) / light_pdf;
                result += Ll * brdf_ * std::max(0.0f, normal.dot((light_pos - hit_pos).normalized()));
            }
        }
    }

    // Round Robin
    float prr = 0.8;
    if (rand() * 1.0 / RAND_MAX > prr)
        return result;

    // sample bxdf
    vec3 wi = hit_obj->mat->sampleBxdf(wo, normal);
    float pdf = hit_obj->mat->pdf(wo, normal, wi);
    vec3 brdf = hit_obj->mat->bxdf(wo, normal, wi, texcoords);
    vec3 Li = trace(hit_pos + wi * 1e-3, wi, triangles, light_sampler, bvh, !is_light_sampled, env_map);
    vec3 contri = Li * abs(wi.dot(normal)) * brdf / pdf / prr;
    result += contri;

    if (contri.norm() > 1000.0f)
    {
        std::stringstream t_stream;
        t_stream << std::fixed << std::setprecision(4) << "Large Value Detected: " << contri << "   " << brdf << "   " << pdf << std::endl;
        std::cerr << t_stream.str();
    }

    return result;
}

vec3 Renderer::traceDepth(const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler, BVH &bvh)
{
    auto [t, b1, b2, hit_obj] = intersect(orig, dir, triangles, bvh);
    return t;
}

vec3 Renderer::traceNormal(const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler, BVH &bvh)
{
    auto [t, b1, b2, hit_obj] = intersect(orig, dir, triangles, bvh);
    if (hit_obj == nullptr)
    {
        return 0.0f;
    }
    vec3 normal = hit_obj->getNormal(b1, b2);
    return normal;
}

void Renderer::prepare(const std::vector<Triangle> &triangles)
{
    qDebug() << "Builing Light Sampler...";
    light_sampler_.initialize(triangles);

    qDebug() << "Builing BVH...";
    bvh_.build(triangles);

    qDebug() << "Prepare finish :)";
}

void Renderer::render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img, int SPP, int img_width, int img_height, std::function<void(bool)> requestDisplayUpdate, std::atomic<int> &con_flag, std::function<void(float)> progress_report, const Texture *env_map)
{

    requestDisplayUpdate(false);
    img = QImage(QSize(img_width, img_height), QImage::Format_RGB888);
    img.fill(Qt::black);

    QTime time;
    time.start();
    auto time_last = time.elapsed();

    vec3 *img_depth = new vec3[img_width * img_height];
    vec3 *img_normal = new vec3[img_width * img_height];

    std::cout << "Rendering... " << std::endl;

    std::atomic<int> pxc = 0;

    auto requestProgressUpdate = [&]()
    {
        float progress = pxc * 1.0f / img_height / img_width;
        progress_report(progress * 100);
        if (time.elapsed() - time_last > 1000)
        {
            std::cout << std::fixed << std::setprecision(2) << "Rendering... " << progress * 100 << "%"
                      << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
            time_last = time.elapsed();
        }
    };

    int block_size = 8;
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
                        vec3 ray_dir = camera.generateRay(x + rand() * 1.0f / RAND_MAX, y + rand() * 1.0f / RAND_MAX, img_width, img_height);
                        result += max(0.0f, trace(camera.pos, ray_dir, triangles, light_sampler_, bvh_, true, env_map));
                    }
                    result /= SPP;
                    // Gamma correction
                    result = result.pow(1.0 / 2.2);
                    result *= 255.0f;
                    img.setPixel(x, y, qRgb(std::min(255.0f, std::max(0.0f, result[0])), std::min(255.0f, std::max(0.0f, result[1])), std::min(255.0f, std::max(0.0f, result[2]))));

                    vec3 ray_dir = camera.generateRay(x + 0.5f, y + 0.5f, img_width, img_height);
                    img_depth[y * img_width + x] = traceDepth(camera.pos, ray_dir, triangles, light_sampler_, bvh_);
                    img_normal[y * img_width + x] = traceNormal(camera.pos, ray_dir, triangles, light_sampler_, bvh_);
                }
            }
            pxc += block_size * block_size;
            requestProgressUpdate();
            // request_disp_update = 1;
        }
    };

    int num_threads = 16;
    std::vector<std::thread> ths;
    for (int i = 0; i < num_threads; i++)
    {
        ths.push_back(std::thread(workerFunc));
    }

    while (pxc < img_width * img_height)
    {
        if (request_disp_update == 1)
        {
            request_disp_update = 0;
            requestDisplayUpdate(false);
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

    // Post processing

    vec3 *img_raw = new vec3[img_width * img_height];
    vec3 *img_raw_new = new vec3[img_width * img_height];
#pragma omp parallel for
    for (int i = 0; i < img_height; i++)
    {
        for (int j = 0; j < img_width; j++)
        {
            QColor color = img.pixelColor(j, i);
            img_raw[(i * img_width + j)][0] = color.red();
            img_raw[(i * img_width + j)][1] = color.green();
            img_raw[(i * img_width + j)][2] = color.blue();
        }
    }

    auto getPix = [&](vec3 *arr, int x, int y) -> vec3
    {
        x = std::max(x, 0);
        y = std::max(y, 0);
        x = std::min(x, img_width - 1);
        y = std::min(y, img_height - 1);
        return arr[y * img_width + x];
    };

#pragma omp parallel for
    for (int y = 0; y < img_height; y++)
    {
        for (int x = 0; x < img_width; x++)
        {
            float sum_weight = 0.0f;
            vec3 color;
            vec3 color_center = getPix(img_raw, x, y);
            vec3 depth_center = getPix(img_depth, x, y);
            vec3 normal_center = getPix(img_depth, x, y);
            for (int dy = -2; dy <= 2; dy++)
            {
                for (int dx = -2; dx <= 2; dx++)
                {
                    vec3 c = getPix(img_raw, x + dx, y + dy);
                    vec3 d = getPix(img_depth, x + dx, y + dy);
                    vec3 n = getPix(img_normal, x + dx, y + dy);
                    float w = exp(-(dx * dx + dy * dy) / (2 * 1.0f)) * exp(-((color_center - c).norm2()) / (2 * 300.0f)) * exp(-((depth_center - d).norm2()) / (2 * 500.0f)) * exp(-((depth_center - d).norm2()) / 1.0f);
                    sum_weight += w;
                    color += w * c;
                }
            }
            img_raw_new[y * img_width + x] = color / sum_weight;
        }
    }

#pragma omp parallel for
    for (int i = 0; i < img_height; i++)
    {
        for (int j = 0; j < img_width; j++)
        {
            vec3 v = img_raw_new[i * img_width + j];
            img.setPixel(j, i, qRgb(v[0], v[1], v[2]));
        }
    }

    delete[] img_raw;
    delete[] img_raw_new;
    delete[] img_depth;
    delete[] img_normal;

    std::cout << std::fixed << std::setprecision(2) << "Rendering... " << 100.0 << "%"
              << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
    con_flag = 0;
    requestDisplayUpdate(true);
}
