#ifndef __RENDERER_NRC_H__
#define __RENDERER_NRC_H__

#include <bits/stdc++.h>
#include "camera.h"
#include "triangle.h"
#include <QImage>
#include "lightsampler.h"
#include "bvh.h"
#include "texture.h"
#include "renderer.h"
#include <QMutex>

using namespace std;

const float rate = 0.003;

struct Neural
{
    float random(float eps = 1)
    {
        return eps / 32768 * rand();
    }

    vector<float> weight;
    float bias;
    float value;
    float raw_value;
    float delta;
    vector<Neural *> prev;

    Neural() : bias(0.00), value(0.00), delta(0.00)
    {
    }

    void forward()
    {
        // Calculate "value" by getting "value"s of "prev"
        float ans = 0;
        for (int i = 0; i < prev.size(); i++)
        {
            float w = weight[i];
            Neural *p = prev[i];
            ans += w * p->value;
        }
        ans += bias;
        raw_value = ans;
        // value = 1 / (1 + exp(-ans));
        value = ans > 0 ? ans : 0.1 * ans;
        delta = 0;
    }

    void backward()
    {
        // Send "delta" to "prev"
        float ad = raw_value >= 0 ? 1 : 0.1;
        for (int i = 0; i < prev.size(); i++)
        {
            float w = weight[i];
            Neural *p = prev[i];
            p->delta += ad * delta * w;
            weight[i] -= ad * delta * p->value * rate;
        }
        bias -= ad * delta * rate;
    }

    void print()
    {
        cout << "bias=" << bias << "\tvalue=" << value << "\tdelta=" << delta << "\tweight=";
        for (int i = 0; i < weight.size(); i++)
            cout << weight[i] << ",";
        cout << endl;
    }

    // usage: set value, reset delta, forward, set delta, backward
};

struct Layer
{
    float random(float eps = 1)
    {
        return eps / 32768 * rand();
    }

    vector<Neural> neurals;

    Layer(int n)
    {
        neurals.resize(n);
    }

    void connect(Layer &prev)
    {
        for (int i = 0; i < neurals.size(); i++)
        {
            for (int j = 0; j < prev.neurals.size(); j++)
            {
                neurals[i].prev.push_back(&prev.neurals[j]);
                neurals[i].weight.push_back((random() - 0.5)*0.2);
            }
        }
    }

    void setValue(const std::vector<float> &values)
    {
        for (int i = 0; i < values.size(); i++)
        {
            neurals[i].value = values[i];
        }
    }

    void setDelta(const std::vector<float> &values)
    {
        for (int i = 0; i < values.size(); i++)
        {
            neurals[i].delta = values[i];
        }
    }

    void setDelta()
    {
        for (int i = 0; i < neurals.size(); i++)
        {
            neurals[i].delta = 0;
        }
    }

    void forward()
    {
        for (int i = 0; i < neurals.size(); i++)
        {
            neurals[i].forward();
        }
    }

    void backward()
    {
        for (int i = 0; i < neurals.size(); i++)
        {
            neurals[i].backward();
        }
    }

    void print()
    {
        for (auto &i : neurals)
        {
            i.print();
        }
        cout << endl;
    }
};

struct MLP
{
    vector<Layer> layers;

    MLP(std::vector<int> shape)
    {
        for (int i = 0; i < shape.size(); i++)
        {
            layers.push_back(Layer(shape[i]));
            if (i > 0)
            {
                layers[i].connect(layers[i - 1]);
            }
        }
    }

    void feed(const std::vector<float> &values)
    {
        layers[0].setValue(values);
        for (int i = 0; i < layers.size(); i++)
        {
            layers[i].setDelta();
        }
    }

    void forward()
    {
        for (int i = 1; i < layers.size(); i++)
        {
            layers[i].forward();
        }
    }

    void backward()
    {
        for (int i = layers.size() - 1; i > 0; i--)
        {
            layers[i].backward();
        }
    }

    std::vector<float> eval(const std::vector<float> &in)
    {
        feed(in);
        forward();
        std::vector<float> out;
        for (int i = 0; i < layers.back().neurals.size(); i++)
        {
            out.push_back(layers.back().neurals[i].value);
        }
        return out;
    }

    float train(const std::vector<float> &in, const std::vector<float> &ans)
    {
        auto out = eval(in);
        float loss = 0;
        for (int i = 0; i < ans.size(); i++)
        {
            float o = out[i];
            float a = ans[i];
            loss += pow(o - a, 2);
            layers.back().neurals[i].delta = o - a;
        }
        backward();

        return loss;
    }

    void print()
    {
        for (auto &i : layers)
        {
            i.print();
        }
    }
};

struct NRC
{
    MLP mlp;

    NRC() : mlp({64, 64, 64, 64, 64, 64, 3})
    {
    }

    float train(const std::vector<float> &in, const vec3 &ans)
    {
        std::vector<float> vans;
        vans.push_back(ans[0]);
        vans.push_back(ans[1]);
        vans.push_back(ans[2]);
        return mlp.train(in, vans);
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
    virtual vec3 trace(NRC &nrc, int depth, bool is_train, Sampler &sampler, const vec3 &orig, const vec3 &dir, const std::vector<Triangle> &triangles, LightSampler &light_sampler_, BVH &bvh_, bool light_source_visible = true, const Envmap *env_map = nullptr);

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
