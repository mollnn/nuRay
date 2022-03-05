#ifndef __RENDERER_BDPT_H__
#define __RENDERER_BDPT_H__

#include <bits/stdc++.h>
#include "camera.h"
#include "triangle.h"
#include <QImage>
#include "lightsampler.h"
#include "bvh.h"
#include "texture.h"
#include "renderer.h"
#include <QMutex>

struct BDPTVertexInfo
{
    const Triangle *primitive;
    vec3 pos;
    float b1,b2;
    vec3 alpha;
    float pf,pr;
};

class RendererBDPT : public Renderer
{
protected:
    // return value of trace is answer for case s=0
    bool visibility(vec3 p, vec3 q, const std::vector<Triangle> &triangles);
    vec3 connect(const std::vector<BDPTVertexInfo> &light_path, const std::vector<BDPTVertexInfo> &eye_path, const std::vector<Triangle> &triangles);
    vec3 trace(std::vector<BDPTVertexInfo> &record, const vec3& initial, Sampler &sampler, const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler_, BVH &bvh_, const Texture *env_map = nullptr);

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
                        const Texture *env_map = nullptr) override;
};

#endif
