#ifndef __RENDERER_BDPT_H__
#define __RENDERER_BDPT_H__

#include <bits/stdc++.h>
#include "camera.h"
#include "triangle.h"
#include <QImage>
#include "samplerstd.h"
#include "lightsampler.h"
#include "bvh.h"
#include "texture.h"
#include "renderer.h"
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
