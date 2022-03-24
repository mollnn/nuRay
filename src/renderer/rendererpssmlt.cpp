#include "../renderer/rendererpssmlt.h"
#include "../renderer/RendererPT.h"
#include "../scene/texture.h"
#include "../sampler/lightsampler.h"
#include "../hierarchy/bvh.h"
#include <QTime>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include "../sampler/samplerpssmlt.h"
// PSSMLT ref Kelemen'01

void RendererPSSMLT::render(const Camera &camera,
                            const std::vector<Triangle> &triangles,
                            QImage &img,
                            Config &config,
                            std::function<void(bool)> display_update_callback,
                            std::atomic<int> &con_flag,
                            std::function<void(float)> progress_report_callback,
                            QMutex &framebuffer_mutex,
                            const Envmap *env_map)
{
    int img_width = config.getValueInt("imgw", 1);
    int img_height = config.getValueInt("imgh", 1);
    int SPP = config.getValueInt("spp", 1);

    auto f = [&](float x) -> int
    {
        return std::max(0.0, std::min(255.0, 255.0f * pow(x, 1.0f / 2.2f)));
    };

    SamplerPSSMLT original_sampler;

    display_update_callback(false);
    framebuffer_mutex.lock();
    img = QImage(QSize(img_width, img_height), QImage::Format_RGB888);
    img.fill(Qt::black);
    framebuffer_mutex.unlock();

    QTime time;
    time.start();
    auto time_last = time.elapsed();

    std::cout << "Rendering... " << std::endl;

    vec3 *render_buf = new vec3[img_width * img_height];

    float b = 0;
    int N_SAMPLE_B = config.getValueInt("bsample", 10000);
    for (int i = 0; i < N_SAMPLE_B; i++)
    {
        original_sampler.newSample();
        float original_r1 = original_sampler.random();
        float original_r2 = original_sampler.random();
        int original_x = original_r1 * img_width, original_y = original_r2 * img_height;
        vec3 original_ray_dir = camera.generateRay(original_r1 * img_width, original_r2 * img_height, img_width, img_height);
        vec3 original_radiance = RendererPT::trace(config, original_sampler, camera.pos, original_ray_dir, triangles, light_sampler_, bvh_, env_map);
        float original_importance = original_radiance.lumin();
        if (!std::isnan(original_importance))
            b += original_importance;
    }
    b /= N_SAMPLE_B;

    std::cout << "b=" << b << std::endl;

    int N = img_width * img_height * SPP;

    float scalar = 1.0 / SPP;

    float large_jump_prob = config.getValueFloat("plargejump", 0.3f);

    //////////////////////////////

    std::atomic<int> progress_unit_counter = 0;

    auto requestProgressUpdate = [&]()
    {
        float progress = progress_unit_counter * 1.0f / N;
        progress_report_callback(progress * 100);
        if (time.elapsed() - time_last > 1000)
        {
            std::cout << std::fixed << std::setprecision(2) << "Rendering... " << progress * 100 << "%"
                      << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
            time_last = time.elapsed();
        }
    };

    std::vector<std::pair<int, int>> task_queue;
    std::mutex buffer_mutex;

    std::atomic request_disp_update = 0;

    std::random_shuffle(task_queue.begin(), task_queue.end());

    int num_threads = config.getValueInt("parallel", 4);
    int n_chain = num_threads;
    int len_chain = (N + n_chain - 1) / n_chain;

    auto workerFunc = [&]()
    {
        SamplerPSSMLT original_sampler;
        original_sampler.newSample();
        SamplerStd std_sampler;

        float original_r1 = original_sampler.random();
        float original_r2 = original_sampler.random();
        int original_x = original_r1 * img_width, original_y = original_r2 * img_height;
        vec3 original_ray_dir = camera.generateRay(original_r1 * img_width, original_r2 * img_height, img_width, img_height);
        vec3 original_radiance = RendererPT::trace(config, original_sampler, camera.pos, original_ray_dir, triangles, light_sampler_, bvh_, env_map);

        int accept_count = 0;
        for (int i = 0; i < len_chain; i++)
        {
            if (con_flag == 0)
            {
                break;
            }

            SamplerPSSMLT tentative_sampler = original_sampler;

            bool large_jump = std_sampler.random() < large_jump_prob;
            tentative_sampler.nextIter(large_jump, config);

            float tentative_r1 = tentative_sampler.random();
            float tentative_r2 = tentative_sampler.random();
            int tentative_x = tentative_r1 * img_width, tentative_y = tentative_r2 * img_height;
            vec3 tentative_ray_dir = camera.generateRay(tentative_r1 * img_width, tentative_r2 * img_height, img_width, img_height);
            vec3 tentative_radiance = RendererPT::trace(config, tentative_sampler, camera.pos, tentative_ray_dir, triangles, light_sampler_, bvh_, env_map);

            float original_importance = original_radiance.lumin() + 1e-9f;
            float tentative_importance = tentative_radiance.lumin() + 1e-9f;
            vec3 original_contrib = original_radiance;
            vec3 tentative_contrib = tentative_radiance;

            float accept_prob = std::max(0.0f, std::min(1.0f, tentative_importance / (original_importance + 1e-9f)));
            // std::cout << tentative_importance / original_importance << " " << original_importance << std::endl;

            float original_weight = (1 - accept_prob) / (original_importance / b + large_jump_prob);
            float tentative_weight = (accept_prob + (large_jump ? 1.0 : 0.0)) / (tentative_importance / b + large_jump_prob);

            original_x = std::max(0, std::min(img_width - 1, original_x));
            original_y = std::max(0, std::min(img_height - 1, original_y));
            tentative_x = std::max(0, std::min(img_width - 1, tentative_x));
            tentative_y = std::max(0, std::min(img_height - 1, tentative_y));

            // render_buf[original_y * img_width + original_x] += bdM * original_contrib / original_importance * b * (1 - accept_prob);
            // render_buf[tentative_y * img_width + tentative_x] += bdM * tentative_contrib / tentative_importance * b * accept_prob;

            buffer_mutex.lock();
            render_buf[original_y * img_width + original_x] += scalar * original_weight * original_contrib;
            render_buf[tentative_y * img_width + tentative_x] += scalar * tentative_weight * tentative_contrib;
            buffer_mutex.unlock();

            if (std_sampler.random() < accept_prob)
            {
                original_sampler = tentative_sampler;
                accept_count++;

                original_r1 = tentative_r1;
                original_r2 = tentative_r2;
                original_x = tentative_x;
                original_y = tentative_y;
                original_ray_dir = tentative_ray_dir;
                original_radiance = tentative_radiance;
            }

            progress_unit_counter++;

            if (progress_unit_counter % 100000 == 0)
            {
                requestProgressUpdate();
                request_disp_update = 1;
            }
        }
    };

    std::vector<std::thread> ths;
    for (int i = 0; i < num_threads; i++)
    {
        ths.push_back(std::thread(workerFunc));
    }

    while (progress_unit_counter < N)
    {
        if (request_disp_update == 1)
        {
            framebuffer_mutex.lock();
            for (int i = 0; i < img_height; i++)
            {
                for (int j = 0; j < img_width; j++)
                {
                    img.setPixel(j, i, qRgb(f(render_buf[i * img_width + j][0]), f(render_buf[i * img_width + j][1]), f(render_buf[i * img_width + j][2])));
                }
            }
            framebuffer_mutex.unlock();
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

    framebuffer_mutex.lock();
    for (int i = 0; i < img_height; i++)
    {
        for (int j = 0; j < img_width; j++)
        {
            img.setPixel(j, i, qRgb(f(render_buf[i * img_width + j][0]), f(render_buf[i * img_width + j][1]), f(render_buf[i * img_width + j][2])));
        }
    }
    framebuffer_mutex.unlock();

    delete[] render_buf;

    std::cout << std::fixed << std::setprecision(2) << "Rendering... " << 100.0 << "%"
              << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
    con_flag = 0;
    display_update_callback(true);
}
