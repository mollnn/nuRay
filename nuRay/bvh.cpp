#include "bvh.h"
#include <QDebug>

std::pair<std::vector<const Triangle *>, std::vector<const Triangle *>> BVH::_divide(const std::vector<const Triangle *> &primitives)
{
    int mid = primitives.size() / 2;
    std::vector<const Triangle *> part_l(primitives.begin(), primitives.begin() + mid);
    std::vector<const Triangle *> part_r(primitives.begin() + mid, primitives.end());
    return {part_l, part_r};
}

BVHNode *BVH::_build(const std::vector<const Triangle *> &primitives)
{
    if (primitives.size() <= 5)
    {
        return new BVHNode(primitives);
    }
    auto [part_l, part_r] = _divide(primitives);
    BVHNode *lc = _build(part_l);
    BVHNode *rc = _build(part_r);
    return new BVHNode(lc, rc);
}

BVH::BVH() : root(nullptr)
{
}

BVH::~BVH()
{
    delete root;
}

void BVH::build(const std::vector<const Triangle *> &primitives)
{
    if (root)
    {
        delete root;
        root = nullptr;
    }
    root = _build(primitives);
}

void BVH::build(const std::vector<Triangle> &primitives)
{
    std::vector<const Triangle *> tmp;
    for (auto &i : primitives)
        tmp.push_back(&i);
    return build(tmp);
}

std::tuple<float, float, float, const Triangle *> BVH::_traversal(const vec3 &origin, const vec3 &dir, BVHNode *p)
{
    if (p == nullptr)
    {
        return {1e18, 0, 0, nullptr};
    }

    if (p->intersect(origin, dir) == false)
    {
        return {1e18, 0, 0, nullptr};
    }

    if (p->left_child == nullptr && p->right_child == nullptr)
    {
        return p->getIntersection(origin, dir);
    }

    auto ans_l = _traversal(origin, dir, p->left_child);
    auto ans_r = _traversal(origin, dir, p->right_child);

    if (std::get<0>(ans_l) > 0 && std::get<0>(ans_l) < std::get<0>(ans_r))
    {
        return ans_l;
    }
    else
    {
        return ans_r;
    }
}

std::tuple<float, float, float, const Triangle *> BVH::intersection(const vec3 &origin, const vec3 &dir)
{
    auto ans = _traversal(origin, dir, root);
    return ans;
}