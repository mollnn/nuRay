#ifndef __RENDERER_BDPT_H__
#define __RENDERER_BDPT_H__

#include <bits/stdc++.h>
#include "../scene/camera.h"
#include "../scene/triangle.h"
#include <QImage>
#include "../sampler/samplerstd.h"
#include "../sampler/lightsampler.h"
#include "../accel/bvh.h"
#include "../scene/texture.h"
#include "../renderer/renderer.h"
#include <QMutex>


struct Vinfo
{
    vec3 c;
    float pf;
    float pr;

    const Triangle *obj;
    vec3 pos;
    vec3 uv;
    float bc1;
    float bc2;
};

Vinfo vInfo(vec3 c, float pf, float pr, const Triangle *obj, vec3 pos, vec3 uv, float bc1, float bc2);

class RendererBDPT : public Renderer
{
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
