#include "renderer.h"
#include "texture.h"

std::tuple<float, float, float, const Triangle *> Renderer::intersect(const vec3 &origin, const vec3 &dir, const std::vector<Triangle> &triangles)
{
    float m_t = 1e18, m_b1 = 0, m_b2 = 0;
    const Triangle *hit_obj = nullptr;
    for (auto &triangle : triangles)
    {
        auto [t, b1, b2] = triangle.intersection(origin, dir);
        if (t < m_t && t > 0 && b1 > 0 && b2 > 0 && b1 + b2 < 1)
        {
            m_t = t;
            m_b1 = b1;
            m_b2 = b2;
            hit_obj = &triangle;
        }
    }
    return {m_t, m_b1, m_b2, hit_obj};
}

vec3 Renderer::trace(const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles)
{
    static Texture test_tex("uvtest.jpg");

    auto [t, b1, b2, hit_obj] = intersect(orig, dir, triangles);
    if (hit_obj == nullptr)
        return vec3(0.0f, 0.0f, 0.0f);

    vec3 normal = hit_obj->getNormal(b1, b2);
    vec3 texcoords = hit_obj->getTexCoords(b1, b2);
    float u = texcoords[0], v = texcoords[1];

    if (normal.dot(dir) > 0)
        return vec3(0.0f, 0.0f, 0.0f); // Back culling, not for refraction

    vec3 hit_pos = orig + dir * t;
    vec3 result;

    // TODO: sample the light
    float brdf_ = 0.3; // todo
    qDebug()<<b1<<" "<<b2<<" "<<u<<" "<<v;
    brdf_ *= test_tex.pixelUV(u,v);

    vec3 light_pos = vec3(0.0f, 2.0f, 4.0f);
    vec3 light_int = vec3(1.0f, 1.0f, 1.0f) * 30.0f;
    vec3 Ll = light_int / ((hit_pos - light_pos).dot(hit_pos - light_pos));
    result += Ll * brdf_ * std::max(0.0f, normal.dot((light_pos - hit_pos).normalized()));

    // Round Robin
    float prr = 0.1;
    if (rand() * 1.0 / RAND_MAX > prr)
        return result;

    // * TODO: Sample brdf (wrap it into Material)
    float r2 = rand() * 1.0 / RAND_MAX;
    float phi = rand() * 1.0 / RAND_MAX * 3.14159 * 2;
    float r = sqrt(r2);
    float h = sqrt(1 - r2);

    vec3 ax0 = abs(normal[0]) < 0.8 ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f);
    vec3 ax1 = ax0.cross(normal).normalized();
    vec3 ax2 = normal.cross(ax1).normalized();
    vec3 wi = h * normal + r * cos(phi) * ax1 + r * sin(phi) * ax2;
    float pdf = 1.0 / 3.14159;
    float brdf = 0.3; // todo
    brdf_ *= test_tex.pixelUV(u,v);
    vec3 Li = trace(hit_pos + wi * 1e-5, wi, triangles);
    result += Li * brdf / pdf / prr;

    return result;
}

void Renderer::render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img)
{
    int SPP = 1;
    for (int y = 0; y < camera.img_height; y++)
    {
        if (y % 10 == 0)
            std::cout << "y=" << y << std::endl;

        for (int x = 0; x < camera.img_width; x++)
        {
            vec3 ray_dir = camera.generateRay(x, y);
            vec3 result;
            for (int i = 0; i < SPP; i++)
            {
                result += trace(camera.pos, ray_dir, triangles) * 255.0f;
            }
            result /= SPP;
            img.setPixel(x, y, qRgb(std::min(255.0f, result[0]), std::min(255.0f, result[1]), std::min(255.0f, result[2])));
        }
    }
}
