#ifndef __RENDERER_PTLS_H__
#define __RENDERER_PTLS_H__

// Path Tracing with Light Sampler

#include <bits/stdc++.h>
#include "camera.h"
#include "triangle.h"
#include <QImage>
#include "lightsampler.h"
#include "bvh.h"
#include "texture.h"
#include "renderer.h"
#include <QMutex>

class RendererPTLS : public Renderer
{
protected:
    virtual vec3 trace(Sampler& sampler, const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler_, BVH &bvh_, bool light_source_visible = true, const Envmap *env_map = nullptr);

public:
    virtual void render(const Camera &camera,
                        const std::vector<Triangle> &triangles,
                        QImage &img,
                        int SPP,
                        int img_width,
                        int img_height,
                        std::function<void(bool)> callback,
                        std::atomic<int> &con_flag,
                        std::function<void(float)> progress_report,
                        QMutex &framebuffer_mutex,
                        const Envmap *env_map = nullptr) override;
};

#endif
