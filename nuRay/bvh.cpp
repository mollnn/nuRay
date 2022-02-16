#include "bvh.h"
#include <QDebug>
#include <bits/stdc++.h>

std::pair<std::vector<const Triangle *>, std::vector<const Triangle *>> BVH::_divide(const std::vector<const Triangle *> &primitives)
{
    // int mid = primitives.size() / 2;
    // std::vector<const Triangle *> part_l(primitives.begin(), primitives.begin() + mid);
    // std::vector<const Triangle *> part_r(primitives.begin() + mid, primitives.end());
    // return {part_l, part_r};

    int ans_axis = 0;
    int ans_mid = 0;
    float ans_cost = 1e38;

    auto prims = primitives;

    auto getSurfaceArea = [](const vec3 &a, const vec3 &b)
    {
        vec3 d = b - a;
        return (d[0] * d[1] + d[1] * d[2] + d[2] * d[0]) * 2;
    };

    for (int axis = 0; axis < 3; axis++)
    {
        std::sort(prims.begin(), prims.end(), [&](const Triangle *a, const Triangle *b)
                  { return a->pMax()[axis] < b->pMax()[axis]; });

        std::vector<float> suffix_surface_area(prims.size());
        vec3 tmp_p_min = 1e18, tmp_p_max = -1e18;
        for (int i = prims.size() - 1; i >= 0; i--)
        {
            tmp_p_min = min(tmp_p_min, prims[i]->pMin());
            tmp_p_max = max(tmp_p_max, prims[i]->pMax());
            suffix_surface_area[i] = getSurfaceArea(tmp_p_min, tmp_p_max);
        }

        tmp_p_min = 1e18, tmp_p_max = -1e18;
        for (int i = 0; i < prims.size() - 1; i++)
        {
            tmp_p_min = min(tmp_p_min, prims[i]->pMin());
            tmp_p_max = max(tmp_p_max, prims[i]->pMax());
            float prefix_surface_area = getSurfaceArea(tmp_p_min, tmp_p_max);
            float cost = prefix_surface_area * (i + 1) + suffix_surface_area[i + 1] * (prims.size() - i);
            if (cost < ans_cost)
            {
                ans_cost = cost;
                ans_mid = i + 1;
                ans_axis = axis;
            }
        }
    }
    std::sort(prims.begin(), prims.end(), [&](const Triangle *a, const Triangle *b)
              { return a->pMax()[ans_axis] < b->pMax()[ans_axis]; });
    std::vector<const Triangle *> part_l(prims.begin(), prims.begin() + ans_mid);
    std::vector<const Triangle *> part_r(prims.begin() + ans_mid, prims.end());
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

BVH::BVH() : root_(nullptr)
{
}

BVH::~BVH()
{
    delete root_;
}

void BVH::build(const std::vector<const Triangle *> &primitives)
{
    if (root_)
    {
        delete root_;
        root_ = nullptr;
    }
    root_ = _build(primitives);
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
    if (root_ == nullptr)
    {
        qDebug() << "null root";
    }
    auto ans = _traversal(origin, dir, root_);
    return ans;
}
