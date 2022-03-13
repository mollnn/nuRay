#ifndef _BVH_H_
#define _BVH_H_

#include "../hierarchy/bvhnode.h"

class BVH
{
    BVHNode *root_;

    std::pair<std::vector<const Triangle *>, std::vector<const Triangle *>> _divide(const std::vector<const Triangle *> &primitives);
    BVHNode *_build(const std::vector<const Triangle *> &primitives);
    std::tuple<float, float, float, const Triangle *> _traversal(const vec3 &origin, const vec3 &dir, BVHNode *p);

public:
    BVH();
    ~BVH();
    void build(const std::vector<const Triangle *> &primitives);
    void build(const std::vector<Triangle> &primitives);
    std::tuple<float, float, float, const Triangle *> intersection(const vec3 &origin, const vec3 &dir);
};

#endif
