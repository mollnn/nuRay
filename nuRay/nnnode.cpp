#include "nnnode.h"

float NNNode::random(float eps)
{
    return eps / 32768 * rand();
}

NNNode::NNNode() : bias(0.00), value(0.00), delta(0.00)
{
}

void NNNode::forward()
{
    // Calculate "value" by getting "value"s of "prev"
    float ans = 0;
    for (int i = 0; i < prev.size(); i++)
    {
        float w = weight[i];
        NNNode *p = prev[i];
        ans += w * p->value;
    }
    ans += bias;
    raw_value = ans;
    // value = 1 / (1 + exp(-ans));
    value = ans > 0 ? ans : 0.1 * ans;
    delta = 0;
}

void NNNode::backward(float rate)
{
    // Send "delta" to "prev"
    float ad = raw_value >= 0 ? 1 : 0.1;
    for (int i = 0; i < prev.size(); i++)
    {
        float w = weight[i];
        NNNode *p = prev[i];
        p->delta += ad * delta * w;
        weight[i] -= ad * delta * p->value * rate;
    }
    bias -= ad * delta * rate;
}

void NNNode::print()
{
    cout << "bias=" << bias << "\tvalue=" << value << "\tdelta=" << delta << "\tweight=";
    for (int i = 0; i < weight.size(); i++)
        cout << weight[i] << ",";
    cout << endl;
}

float NNNodeLayer::random(float eps)
{
    return eps / 32768 * rand();
}

NNNodeLayer::NNNodeLayer(int n)
{
    neurals.resize(n);
}

void NNNodeLayer::connect(NNNodeLayer &prev)
{
    for (int i = 0; i < neurals.size(); i++)
    {
        for (int j = 0; j < prev.neurals.size(); j++)
        {
            neurals[i].prev.push_back(&prev.neurals[j]);
            neurals[i].weight.push_back((random() - 0.5) * 0.2);
        }
    }
}

void NNNodeLayer::setValue(const std::vector<float> &values)
{
    for (int i = 0; i < values.size(); i++)
    {
        neurals[i].value = values[i];
    }
}

void NNNodeLayer::setDelta(const std::vector<float> &values)
{
    for (int i = 0; i < values.size(); i++)
    {
        neurals[i].delta = values[i];
    }
}

void NNNodeLayer::setDelta()
{
    for (int i = 0; i < neurals.size(); i++)
    {
        neurals[i].delta = 0;
    }
}

void NNNodeLayer::forward()
{
    for (int i = 0; i < neurals.size(); i++)
    {
        neurals[i].forward();
    }
}

void NNNodeLayer::backward(float rate)
{
    for (int i = 0; i < neurals.size(); i++)
    {
        neurals[i].backward(rate);
    }
}

void NNNodeLayer::print()
{
    for (auto &i : neurals)
    {
        i.print();
    }
    cout << endl;
}

NNNodeMultiLayerPerceptron::NNNodeMultiLayerPerceptron(std::vector<int> shape)
{
    for (int i = 0; i < shape.size(); i++)
    {
        layers.push_back(NNNodeLayer(shape[i]));
        if (i > 0)
        {
            layers[i].connect(layers[i - 1]);
        }
    }
}

void NNNodeMultiLayerPerceptron::feed(const std::vector<float> &values)
{
    layers[0].setValue(values);
    for (int i = 0; i < layers.size(); i++)
    {
        layers[i].setDelta();
    }
}

void NNNodeMultiLayerPerceptron::forward()
{
    for (int i = 1; i < layers.size(); i++)
    {
        layers[i].forward();
    }
}

void NNNodeMultiLayerPerceptron::backward(float rate)
{
    for (int i = layers.size() - 1; i > 0; i--)
    {
        layers[i].backward(rate);
    }
}

std::vector<float> NNNodeMultiLayerPerceptron::eval(const std::vector<float> &in)
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

float NNNodeMultiLayerPerceptron::train(const std::vector<float> &in, const std::vector<float> &ans, float rate)
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
    backward(rate);

    return loss;
}

void NNNodeMultiLayerPerceptron::print()
{
    for (auto &i : layers)
    {
        i.print();
    }
}