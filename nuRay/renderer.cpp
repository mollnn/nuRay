#include "renderer.h"
#include "texture.h"
#include "lightsampler.h"
#include "bvh.h"
#include <QTime>
#include <QDebug>

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

vec3 Renderer::trace(const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler, BVH &bvh, bool light_source_visible)
{
    auto [t, b1, b2, hit_obj] = intersect(orig, dir, triangles, bvh);
    if (hit_obj == nullptr)
        return vec3(0.0f, 0.0f, 0.0f);

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

    if (normal.dot(dir) > 0)
        return vec3(0.0f, 0.0f, 0.0f); // ! Back culling, not for refraction

    vec3 hit_pos = orig + dir * t;
    vec3 result;

    // sample the light
    bool is_light_sampled = false;
    if (hit_obj->mat->requireLightSampling(wo, normal))
    {
        is_light_sampled = true;
        const Triangle *light_obj = light_sampler.sampleLight();
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

    // Round Robin
    float prr = 0.8;
    if (rand() * 1.0 / RAND_MAX > prr)
        return result;

    // sample bxdf
    vec3 wi = hit_obj->mat->sampleBxdf(wo, normal);
    float pdf = hit_obj->mat->pdf(wo, normal, wi);
    vec3 brdf = hit_obj->mat->bxdf(wo, normal, wi, texcoords);
    vec3 Li = trace(hit_pos + wi * 1e-3, wi, triangles, light_sampler, bvh, !is_light_sampled);
    result += Li * wi.dot(normal) * brdf / pdf / prr;

    return result;
}

void Renderer::prepare(const std::vector<Triangle> &triangles)
{
    qDebug() << "Builing Light Sampler...";
    light_sampler.initialize(triangles);

    qDebug() << "Builing BVH...";
    bvh.build(triangles);

    qDebug() << "Prepare finish :)";
}

void Renderer::render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img, int SPP, int img_width, int img_height)
{
    img = QImage(QSize(img_width, img_height), QImage::Format_RGB888);

    QTime time;
    time.start();
    auto time_last = time.elapsed();

    std::cout << "Rendering... " << std::endl;
    for (int y = 0; y < img_height; y++)
    {
        float progress = y * 1.0f / img_height;
        if (time.elapsed() - time_last > 1000)
        {
            std::cout << std::fixed << std::setprecision(2) << "Rendering... " << progress * 100 << "%"
                      << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
            time_last = time.elapsed();
        }

#pragma omp parallel for
        for (int x = 0; x < img_width; x++)
        {
            vec3 result;
            for (int i = 0; i < SPP; i++)
            {
                vec3 ray_dir = camera.generateRay(x + rand() * 1.0f / RAND_MAX, y + rand() * 1.0f / RAND_MAX, img_width, img_height);
                result += max(0.0f, trace(camera.pos, ray_dir, triangles, light_sampler, bvh));
            }
            result /= SPP;
            // Gamma correction
            result = result.pow(1.0 / 2.2);
            result *= 255.0f;
            img.setPixel(x, y, qRgb(std::min(255.0f, std::max(0.0f, result[0])), std::min(255.0f, std::max(0.0f, result[1])), std::min(255.0f, std::max(0.0f, result[2]))));
        }
    }
    std::cout << std::fixed << std::setprecision(2) << "Rendering... " << 100.0 << "%"
              << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
}
