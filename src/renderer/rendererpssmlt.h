#ifndef RENDERERPSSMLT_H
#define RENDERERPSSMLT_H

#include <bits/stdc++.h>
#include "../scene/camera.h"
#include "../scene/triangle.h"
#include <QImage>
#include "../sampler/lightsampler.h"
#include "../hierarchy/bvh.h"
#include "../scene/texture.h"
#include "../renderer/renderer.h"
#include "../renderer/rendererpt.h"
#include <QMutex>


class RendererPSSMLT: public RendererPT
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

#endif // RENDERERPSSMLT_H
