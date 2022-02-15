#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <bits/stdc++.h>
#include "camera.h"
#include "triangle.h"
#include <QImage>
#include "lightsampler.h"
#include "bvh.h"

class Renderer
{
    std::tuple<float, float, float, const Triangle *> intersect(const vec3 &origin, const vec3 &dir, const std::vector<Triangle> &triangles, BVH &bvh);
    vec3 trace(const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler, BVH &bvh, bool light_source_visible = true);

public:
    void prepare(const std::vector<Triangle> &triangles);
    void render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img);

private:
    BVH bvh;
    LightSampler light_sampler;
};

#endif