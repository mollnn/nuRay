#include "renderernrc.h"
#include "texture.h"
#include "lightsampler.h"
#include "bvh.h"
#include <QTime>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include "samplerstd.h"

float loss_acc = 0.0f, train_acc = 0.0f;

vec3 RendererNRC::trace(NRC &nrc, int depth, bool is_train, Sampler &sampler, const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler, BVH &bvh, bool light_source_visible, const Envmap *env_map)
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

    // Begin of shading

    vec3 hit_pos = orig + dir * t;
    vec3 result;

    auto encoderSine = [&](float x, float f) -> float
    {
        return 2 * abs(fmod(x / f, 2) - 1) - 1;
    };

    auto encoderFreq = [&](float x) -> std::vector<float>
    {
        std::vector<float> ans;
        for (int i = 0; i < 12; i++)
        {
            ans.push_back(encoderSine(x, std::pow(2, i)));
        }
        return ans;
    };

    auto quartic = [&](float x) -> float
    {
        if (x < -1 || x > 1)
            return 0.0f;
        return 15.0f / 16.0f * std::pow(1 - x * x, 2);
    };

    auto quarticUnit = [&](float x, int i, int n) -> float
    {
        return quartic(2 * n * x - 2 * i - 1);
    };

    auto encoderQuartic = [&](float x) -> std::vector<float>
    {
        // input range [0,1]
        std::vector<float> ans;
        for (int i = 0; i < 4; i++)
        {
            ans.push_back(quarticUnit(x, i, 4));
        }
        return ans;
    };

    auto encoderId = [&](float x) -> std::vector<float>
    {
        return {x};
    };

    // *: calculate encoding of hit_pos and wo and ...
    std::vector<float> encoding;

    auto pushEncoding = [&](const std::vector<float> &x)
    {
        for (auto i : x)
            encoding.push_back(i);
    };

    float rough = hit_obj->mat->roughness(texcoords);
    vec3 reflectance_d = hit_obj->mat->reflectanceDiffuse(texcoords);
    vec3 reflectance_s = hit_obj->mat->reflectanceSpecular(texcoords);

    pushEncoding(encoderFreq(hit_pos[0]));
    pushEncoding(encoderFreq(hit_pos[1]));
    pushEncoding(encoderFreq(hit_pos[2]));
    pushEncoding(encoderQuartic(wo[0] * 0.5f + 0.5f));
    pushEncoding(encoderQuartic(wo[1] * 0.5f + 0.5f));
    pushEncoding(encoderQuartic(normal[0] * 0.5f + 0.5f));
    pushEncoding(encoderQuartic(normal[1] * 0.5f + 0.5f));
    pushEncoding(encoderQuartic(exp(-rough)));
    pushEncoding(encoderId(reflectance_d[0]));
    pushEncoding(encoderId(reflectance_d[1]));
    pushEncoding(encoderId(reflectance_d[2]));
    pushEncoding(encoderId(reflectance_s[0]));
    pushEncoding(encoderId(reflectance_s[1]));
    pushEncoding(encoderId(reflectance_s[2]));

    while (encoding.size() < 64)
    {
        encoding.push_back(0.0f);
    }

    // *: decide whether to return with cache
    if (depth >= 2 && (!is_train || depth >= 4))
    {
        return nrc.eval(encoding);
    }

    // sample the light
    bool is_light_sampled = false;
    if (hit_obj->mat->requireLightSampling(wo, normal))
    {
        is_light_sampled = true;
        const Triangle *light_obj = light_sampler.sampleLight(sampler);
        if (light_obj != nullptr)
        {
            auto [light_pos, light_bc1, light_bc2] = light_obj->sample(sampler);
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
    if (sampler.random() < prr)
    {
        // sample bxdf
        vec3 wi = hit_obj->mat->sampleBxdf(sampler, wo, normal);
        float pdf = hit_obj->mat->pdf(wo, normal, wi);
        vec3 brdf = hit_obj->mat->bxdf(wo, normal, wi, texcoords);
        vec3 Li = trace(nrc, depth + 1, is_train, sampler, hit_pos + wi * 1e-3, wi, triangles, light_sampler, bvh, !is_light_sampled, env_map);
        vec3 contri = Li * abs(wi.dot(normal)) * brdf / pdf / prr;
        result += contri;
    }

    // *: update cache if is training

    if (is_train)
    {
        float loss = nrc.train(encoding, result);
        loss_acc += loss;
        train_acc += 1.0f;
    }

    return result;
}

void RendererNRC::render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img, int SPP, int img_width, int img_height, std::function<void(bool)> requestDisplayUpdate, std::atomic<int> &con_flag, std::function<void(float)> progress_report, QMutex &framebuffer_mutex, const Envmap *env_map)
{
    SamplerStd sampler;

    NRC nrc;

    requestDisplayUpdate(false);
    framebuffer_mutex.lock();
    img = QImage(QSize(img_width, img_height), QImage::Format_RGB888);
    img.fill(Qt::black);
    framebuffer_mutex.unlock();

    QTime time;
    time.start();
    auto time_last = time.elapsed();

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

    vec3 *buf = new vec3[img_width * img_height];

    const int PRE = 1;

    for (int i = 0; i < PRE; i++)
    {
        for (int y = 0; y < img_height; y++)
        {
            for (int x = 0; x < img_width; x++)
            {
                vec3 ray_dir = camera.generateRay(x + sampler.random(), y + sampler.random(), img_width, img_height);
                buf[y * img_width + x] += max(0.0f, trace(nrc, 0, true, sampler, camera.pos, ray_dir, triangles, light_sampler_, bvh_, true, env_map));
            }
        }
        std::cout << "pre i=" << i << " loss=" << loss_acc / train_acc << std::endl;
        loss_acc = 0;
        train_acc = 0;
    }

    for (int i = 0; i < SPP; i++)
    {
        for (int y = 0; y < img_height; y++)
        {
            for (int x = 0; x < img_width; x++)
            {
                vec3 ray_dir = camera.generateRay(x + sampler.random(), y + sampler.random(), img_width, img_height);
                buf[y * img_width + x] += max(0.0f, trace(nrc, 0, sampler.random() < 0.2, sampler, camera.pos, ray_dir, triangles, light_sampler_, bvh_, true, env_map));
            }
        }
        std::cout << "i=" << i << " loss=" << loss_acc / train_acc << std::endl;
        loss_acc = 0;
        train_acc = 0;
    }
    for (int y = 0; y < img_height; y++)
    {
        for (int x = 0; x < img_width; x++)
        {
            auto result = buf[y * img_width + x];
            result /= SPP;
            result = result.pow(1.0 / 2.2);
            result *= 255.0f;
            img.setPixel(x, y, qRgb(std::min(255.0f, std::max(0.0f, result[0])), std::min(255.0f, std::max(0.0f, result[1])), std::min(255.0f, std::max(0.0f, result[2]))));

            vec3 ray_dir = camera.generateRay(x + 0.5f, y + 0.5f, img_width, img_height);
        }
    }

    std::cout << std::fixed << std::setprecision(2) << "Rendering... " << 100.0 << "%"
              << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
    con_flag = 0;
    requestDisplayUpdate(true);
}
