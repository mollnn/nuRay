#ifndef _BVH_NODE_H_
#define _BVH_NODE_H_

#include "../math/vec3.h"
#include "../scene/triangle.h"
#include <bits/stdc++.h>

struct BVHNode
{
    vec3 p_min, p_max;
    BVHNode *left_child, *right_child;
    std::vector<const Triangle *> primitives;

    BVHNode() = default;
    ~BVHNode();
    BVHNode(const std::vector<const Triangle *> &triangles);
    BVHNode(BVHNode *lc, BVHNode *rc);

    // Test intersection with AABB
    bool intersect(const vec3 &origin, const vec3 &dir);
    // Get intersection from primitive list (only for leaf!)
    // Return payload: distance, bc1, bc2, primitive pointer
    std::tuple<float, float, float, const Triangle *> getIntersection(const vec3 &origin, const vec3 &dir);
};

#endif
