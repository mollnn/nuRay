#include "rendererpssmlt.h"
#include "rendererptls.h"
#include "texture.h"
#include "lightsampler.h"
#include "bvh.h"
#include <QTime>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include "samplerpssmlt.h"
// PSSMLT ref Kelemen'01

void RendererPSSMLT::render(const Camera &camera,
                            const std::vector<Triangle> &triangles,
                            QImage &img,
                            int SPP,
                            int img_width,
                            int img_height,
                            std::function<void(bool)> requestDisplayUpdate,
                            std::atomic<int> &con_flag,
                            std::function<void(float)> progress_report,
                            QMutex &framebuffer_mutex,
                            const Envmap *env_map)
{
    SamplerPSSMLT sampler;

    requestDisplayUpdate(false);
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
    for (int i = 0; i < 10000; i++)
    {
        sampler.newSample();
        SamplerPSSMLT &original_sampler = sampler;
        float original_r1 = original_sampler.random();
        float original_r2 = original_sampler.random();
        int original_x = original_r1 * img_width, original_y = original_r2 * img_height;
        vec3 original_ray_dir = camera.generateRay(original_r1 * img_width, original_r2 * img_height, img_width, img_height);
        vec3 original_radiance = RendererPT::trace(original_sampler, camera.pos, original_ray_dir, triangles, light_sampler_, bvh_, env_map);
        float original_importance = original_radiance.norm();
        if (!std::isnan(original_importance))
            b += original_importance;
    }
    b /= 10000;
    int N = img_width * img_height * SPP;
    float bdM = b / N * img_width * img_height;
    std::cout << "bdM=" << bdM << std::endl;

    float large_jump_prob = 0.2f;

    sampler.newSample();
    SamplerStd std_sampler;
    for (int i = 0; i < N; i++)
    {
        SamplerPSSMLT &original_sampler = sampler;
        SamplerPSSMLT tentative_sampler = sampler;

        bool large_jump = std_sampler.random() < large_jump_prob;
        tentative_sampler.nextIter(large_jump);

        float original_r1 = original_sampler.random();
        float original_r2 = original_sampler.random();
        int original_x = original_r1 * img_width, original_y = original_r2 * img_height;
        vec3 original_ray_dir = camera.generateRay(original_r1 * img_width, original_r2 * img_height, img_width, img_height);
        vec3 original_radiance = RendererPT::trace(original_sampler, camera.pos, original_ray_dir, triangles, light_sampler_, bvh_, env_map);

        float tentative_r1 = tentative_sampler.random();
        float tentative_r2 = tentative_sampler.random();
        int tentative_x = tentative_r1 * img_width, tentative_y = tentative_r2 * img_height;
        vec3 tentative_ray_dir = camera.generateRay(tentative_r1 * img_width, tentative_r2 * img_height, img_width, img_height);
        vec3 tentative_radiance = RendererPT::trace(tentative_sampler, camera.pos, tentative_ray_dir, triangles, light_sampler_, bvh_, env_map);

        // TODO: use luminance rather than algebra norm
        float original_importance = original_radiance.norm() + 1e-8f;
        float tentative_importance = tentative_radiance.norm() + 1e-8f;
        vec3 original_contrib = original_radiance / original_importance;
        vec3 tentative_contrib = tentative_radiance / tentative_importance;

        float accept_prob = std::min(1.0f, tentative_importance / original_importance);

        vec3 original_weight = (1 - accept_prob) / (original_importance / b + large_jump_prob);
        vec3 tentative_weight = (accept_prob + large_jump) / (tentative_importance / b + large_jump_prob);

        original_x = std::max(0, std::min(img_width - 1, original_x));
        original_y = std::max(0, std::min(img_height - 1, original_y));
        tentative_x = std::max(0, std::min(img_width - 1, tentative_x));
        tentative_y = std::max(0, std::min(img_height - 1, tentative_y));
        render_buf[original_y * img_width + original_x] += bdM * original_weight * original_contrib * (1 - accept_prob);
        render_buf[tentative_y * img_width + tentative_x] += bdM * tentative_weight * tentative_contrib * accept_prob;

        float accept_r = rand() * 1.0f / RAND_MAX;
        if (accept_r < accept_prob)
        {
            sampler = tentative_sampler;
        }

        if (i % 5000 == 0)
        {
            std::cout << i << " / " << 20000 << std::endl;
        }
    }

    auto f = [&](float x) -> int
    {
        return std::max(0.0, std::min(255.0, 255.0f * pow(x, 1.0f / 2.2f)));
    };

    for (int i = 0; i < img_height; i++)
    {
        for (int j = 0; j < img_width; j++)
        {
            img.setPixel(j, i, qRgb(f(render_buf[i * img_width + j][0]), f(render_buf[i * img_width + j][1]), f(render_buf[i * img_width + j][2])));
        }
    }

    delete[] render_buf;

    std::cout << std::fixed << std::setprecision(2) << "Rendering... " << 100.0 << "%"
              << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
    con_flag = 0;
    requestDisplayUpdate(true);
}
