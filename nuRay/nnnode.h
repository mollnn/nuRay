#ifndef NNNODE_H
#define NNNODE_H

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

// Implementation here treat neural network as graph
// Tensor-based implementations will be faster definately

struct NNNode
{
    float random(float eps = 1);

    vector<float> weight;
    float bias;
    float value;
    float raw_value;
    float delta;
    vector<NNNode *> prev;

    NNNode();

    void forward();

    void backward(float rate);

    void print();

    // usage: set value, reset delta, forward, set delta, backward
};

struct NNNodeLayer
{
    float random(float eps = 1);

    vector<NNNode> neurals;

    NNNodeLayer(int n);

    void connect(NNNodeLayer &prev);

    void setValue(const std::vector<float> &values);

    void setDelta(const std::vector<float> &values);

    void setDelta();

    void forward();

    void backward(float rate);

    void print();
};

struct NNNodeMultiLayerPerceptron
{
    vector<NNNodeLayer> layers;

    NNNodeMultiLayerPerceptron(std::vector<int> shape);

    void feed(const std::vector<float> &values);

    void forward();

    void backward(float rate);

    std::vector<float> eval(const std::vector<float> &in);

    float train(const std::vector<float> &in, const std::vector<float> &ans, float rate);

    void print();
};

#endif // NNNODE_H
