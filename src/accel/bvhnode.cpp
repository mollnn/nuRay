#include "../accel/bvhnode.h"
#include <QDebug>

BVHNode::BVHNode(const std::vector<const Triangle *> &triangles) : left_child(nullptr),
                                                                   right_child(nullptr),
                                                                   p_min(1e18),
                                                                   p_max(-1e18)
{
    for (auto triangle : triangles)
    {
        for (int i = 0; i < 3; i++)
        {
            p_min = min(p_min, triangle->p[i]);
            p_max = max(p_max, triangle->p[i]);
        }
    }
    primitives = triangles;
}

BVHNode::~BVHNode()
{
    if (left_child)
        delete left_child;
    if (right_child)
        delete right_child;
}

BVHNode::BVHNode(BVHNode *lc, BVHNode *rc) : left_child(lc),
                                             right_child(rc),
                                             p_min(min(lc->p_min, rc->p_min)),
                                             p_max(max(lc->p_max, rc->p_max))
{
}

bool BVHNode::intersect(const vec3 &origin, const vec3 &dir)
{
    float t_in = -1e18, t_out = 1e18;
    for (int i = 0; i < 3; i++)
    {
        float mn = p_min[i], mx = p_max[i];
        float t1 = (mn - origin[i]) / (dir[i] + 1e-9f), t2 = (mx - origin[i]) / (dir[i] + 1e-9f);
        if (t1 > t2)
            std::swap(t1, t2);
        t_in = std::max(t_in, t1);
        t_out = std::min(t_out, t2);
    }
    return t_in <= t_out && t_out >= 0;
}

std::tuple<float, float, float, const Triangle *> BVHNode::getIntersection(const vec3 &origin, const vec3 &dir)
{
    float m_t = 1e18, m_b1 = 0, m_b2 = 0;
    const Triangle *hit_obj = nullptr;
    for (auto triangle : primitives)
    {
        auto [t, b1, b2] = triangle->intersection(origin, dir);
        if (t < m_t && t > 0 && b1 >= 0 && b2 >= 0 && b1 + b2 <= 1)
        {
            m_t = t;
            m_b1 = b1;
            m_b2 = b2;
            hit_obj = triangle;
        }
    }
    return {m_t, m_b1, m_b2, hit_obj};
}