#ifndef __RENDERER_PTLS_H__
#define __RENDERER_PTLS_H__

// Path Tracing with Light Sampler

#include <bits/stdc++.h>
#include "../scene/camera.h"
#include "../scene/triangle.h"
#include <QImage>
#include "../sampler/lightsampler.h"
#include "../accel/bvh.h"
#include "../scene/texture.h"
#include "../renderer/renderer.h"
#include <QMutex>

class RendererPTLS : public Renderer
{
protected:
    virtual vec3 trace(Config& config,Sampler& sampler, const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler_, BVH &bvh_, float light_fac_mis, const Envmap *env_map = nullptr);

public:
    virtual void render(const Camera &camera,
                        const std::vector<Triangle> &triangles,
                        QImage &img,
                        Config &config,
                        std::function<void(bool)> callback,
                        std::atomic<int> &con_flag,
                        std::function<void(float)> progress_report_callback,
                        QMutex &framebuffer_mutex,
                        const Envmap *env_map = nullptr) override;
};

#endif
