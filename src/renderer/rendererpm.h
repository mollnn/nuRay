#ifndef __RENDERER_PM_H__
#define __RENDERER_PM_H__

#include <bits/stdc++.h>
#include "../scene/camera.h"
#include "../scene/triangle.h"
#include <QImage>
#include "../sampler/lightsampler.h"
#include "../hierarchy/bvh.h"
#include "../scene/texture.h"
#include "../renderer/renderer.h"
#include <QMutex>
#include "../hierarchy/kdtree.h"

class RendererPM : public Renderer
{
    struct Photon
    {
        vec3 pos;
        vec3 dir;
        vec3 power;
    };

protected:
    virtual vec3 trace(const KDTree<Photon>& photon_map, Sampler &sampler, const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler_, BVH &bvh_, const Envmap *env_map = nullptr);

public:
    virtual void render(const Camera &camera,
                        const std::vector<Triangle> &triangles,
                        QImage &img,
                        Config &config,
                        std::function<void(bool)> callback,
                        std::atomic<int> &con_flag,
                        std::function<void(float)> progress_report,
                        QMutex &framebuffer_mutex,
                        const Envmap *env_map = nullptr) override;
};

#endif
