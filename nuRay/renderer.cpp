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



void Renderer::prepare(const std::vector<Triangle> &triangles)
{
    qDebug() << "Builing Light Sampler...";
    light_sampler_.initialize(triangles);

    qDebug() << "Builing BVH...";
    bvh_.build(triangles);

    qDebug() << "Prepare finish :)";
}
