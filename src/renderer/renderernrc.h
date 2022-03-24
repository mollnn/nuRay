#ifndef __RENDERER_NRC_H__
#define __RENDERER_NRC_H__

#include <bits/stdc++.h>
#include "../scene/camera.h"
#include "../scene/triangle.h"
#include <QImage>
#include "../sampler/lightsampler.h"
#include "../hierarchy/bvh.h"
#include "../scene/texture.h"
#include "../renderer/renderer.h"
#include <QMutex>
#include "../nn/nnnode.h"

using namespace std;

struct NeuralRadianceCache
{
    NNNodeMultiLayerPerceptron mlp;

    NeuralRadianceCache() : mlp({64, 64, 64, 64, 64, 64, 3})
    {
    }

    float train(const std::vector<float> &in, const vec3 &ans, float rate)
    {
        std::vector<float> vans;
        vans.push_back(ans[0]);
        vans.push_back(ans[1]);
        vans.push_back(ans[2]);
        return mlp.train(in, vans, rate);
    }

    vec3 eval(const std::vector<float> &in)
    {
        auto tmp = mlp.eval(in);
        // std::cout << "eval " << vec3(tmp[0], tmp[1], tmp[2]) << " from ";
        // for (auto i : in)
        //     std::cout << i << " ";
        // std::cout << endl;
        return max(vec3(tmp[0], tmp[1], tmp[2]), 0.0f);
    }
};

class RendererNRC : public Renderer
{

protected:
    virtual vec3 trace(Config& config, NeuralRadianceCache &nrc, int depth, bool is_train, Sampler &sampler, const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler_, BVH &bvh_, bool light_source_visible = true, const Envmap *env_map = nullptr);

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
