#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <bits/stdc++.h>
#include "camera.h"
#include "triangle.h"
#include <QImage>
#include "lightsampler.h"
#include "bvh.h"
#include "texture.h"
#include <QMutex>

class Renderer
{
protected:
    std::tuple<float, float, float, const Triangle *> intersect(const vec3 &origin, const vec3 &dir, const std::vector<Triangle> &triangles, BVH &bvh_);

public:
    virtual void prepare(const std::vector<Triangle> &triangles);
    virtual void render(const Camera &camera,
                const std::vector<Triangle> &triangles, 
                QImage &img, int SPP, int img_width, int img_height, 
                std::function<void(bool)> callback, std::atomic<int> &con_flag, 
                std::function<void(float)> progress_report, 
                QMutex& framebuffer_mutex,
                const Texture *env_map = nullptr) = 0;

protected:
    BVH bvh_;
    LightSampler light_sampler_;
};

#endif
