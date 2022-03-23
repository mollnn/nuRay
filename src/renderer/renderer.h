#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <bits/stdc++.h>
#include "../scene/camera.h"
#include "../scene/triangle.h"
#include <QImage>
#include "../sampler/lightsampler.h"
#include "../hierarchy/bvh.h"
#include "../scene/texture.h"
#include "../scene/envmap.h"
#include "../utils/config.h"
#include <QMutex>

class Renderer
{
protected:
    std::tuple<float, float, float, const Triangle *> intersect(const vec3 &origin, const vec3 &dir, const std::vector<Triangle> &triangles, BVH &bvh_);

public:
    virtual void prepare(const std::vector<Triangle> &triangles);
    virtual void render(const Camera &camera,
                        const std::vector<Triangle> &triangles,
                        QImage &img,
                        Config &config,
                        std::function<void(bool)> callback, std::atomic<int> &con_flag,
                        std::function<void(float)> progress_report,
                        QMutex &framebuffer_mutex,
                        const Envmap *env_map = nullptr) = 0;
    virtual void setEnvmap(const Envmap &env_map);

protected:
    BVH bvh_;
    LightSampler light_sampler_;
};

#endif
