#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <bits/stdc++.h>
#include "camera.h"
#include "triangle.h"
#include <QImage>
#include "lightsampler.h"
#include "bvh.h"
#include "texture.h"

class Renderer
{
    std::tuple<float, float, float, const Triangle *> intersect(const vec3 &origin, const vec3 &dir, const std::vector<Triangle> &triangles, BVH &bvh_);
    vec3 trace(const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler_, BVH &bvh_, bool light_source_visible = true, const Texture *env_map = nullptr);
    vec3 traceDepth(const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler, BVH &bvh);
    vec3 traceNormal(const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler, BVH &bvh);

public:
    void prepare(const std::vector<Triangle> &triangles);
    void render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img, int SPP, int img_width, int img_height, std::function<void(bool)> callback, std::atomic<int>& con_flag, std::function<void(float)> progress_report, const Texture *env_map = nullptr);

private:
    BVH bvh_;
    LightSampler light_sampler_;

};

#endif
