#include "../renderer/rendererpt.h"
#include "../scene/texture.h"
#include "../accel/bvh.h"
#include <QTime>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include "../sampler/samplerstd.h"

// Path Tracing without Light Sampler

vec3 RendererPT::trace(Config& config, Sampler &sampler, const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler, BVH &bvh, const Envmap *env_map)
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
    vec3 result;

    if (hit_obj->mat->isEmission())
    {
        result += hit_obj->mat->emission(wo, normal);
    }

    vec3 texcoords = hit_obj->getTexCoords(b1, b2);
    float u = texcoords[0], v = texcoords[1];

    if (normal.dot(dir) > 0 && hit_obj->mat->isTransmission() == false)
        return vec3(0.0f, 0.0f, 0.0f);

    vec3 hit_pos = orig + dir * t;

    // Round Robin
    float prr = config.getValueFloat("prr", 0.8f);
    if (sampler.random() > prr)
        return result;

    // sample bxdf
    vec3 wi = hit_obj->mat->sampleBxdf(sampler, wo, normal);
    float pdf = hit_obj->mat->pdf(wo, normal, wi);
    vec3 brdf = hit_obj->mat->bxdf(wo, normal, wi, texcoords);
    vec3 Li = trace(config, sampler, hit_pos + wi * 1e-3, wi, triangles, light_sampler, bvh, env_map);
    vec3 contri = Li * abs(wi.dot(normal)) * brdf / pdf / prr;
    result += contri;

    return result;
}

void RendererPT::render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img, Config &config, std::function<void(bool)> display_update_callback, std::atomic<int> &con_flag, std::function<void(float)> progress_report_callback, QMutex &framebuffer_mutex, const Envmap *env_map)
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

    int block_size = config.getValueInt("blocksize",4);
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
                        result += max(0.0f, trace(config, sampler, camera.pos, ray_dir, triangles, light_sampler_, bvh_, env_map));
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

    int num_threads = config.getValueInt("parallel",4);
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
