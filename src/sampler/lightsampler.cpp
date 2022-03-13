#include "../sampler/lightsampler.h"
#include "../sampler/envmapsampler.h"

void LightSampler::setPrimitives(const std::vector<Triangle> &scene)
{
    lighting_triangles_.clear();
    cdf_.clear();
    pdf_.clear();

    // Policy of probability distribution: Area currently

    float sum_weight = 0;
    for (const auto &triangle : scene)
    {
        if (triangle.mat->isEmission())
        {
            float weight = triangle.area();
            pdf_.push_back(weight);
            sum_weight += weight;
            lighting_triangles_.push_back(&triangle);
        }
    }
    sum_weight += 1e-8;
    for (auto &x : pdf_)
        x /= sum_weight;
    sum_weight_primitives_ = sum_weight;

    float sum_prob = 1e-6;
    for (int i = 0; i < pdf_.size(); i++)
    {
        sum_prob += pdf_[i];
        cdf_.push_back(sum_prob);
    }

    if (lighting_triangles_.size() == 0)
    {
        std::cerr << "LightSampler::initialize(...) called but no light" << std::endl;
    }
}

void LightSampler::setEnvmap(const Envmap &envmap)
{
    envmap_ = &envmap;
    EnvmapSampler es;
    es.initialize(envmap);
    sum_weight_envmap_ = es.weight() * 4 * 3.14159;
}

const Triangle *LightSampler::sampleLight(Sampler &sampler)
{
    if (lighting_triangles_.size() == 0)
    {
        return nullptr;
    }
    float r = sampler.random();
    int id = lower_bound(cdf_.begin(), cdf_.end(), r) - cdf_.begin();
    return lighting_triangles_[id];
}

std::tuple<vec3, vec3, vec3> LightSampler::sampleAllLight(Sampler &sampler)
{
    float p0 = sum_weight_envmap_ / (sum_weight_envmap_ + sum_weight_primitives_);
    // if (sampler.random() < p0)
    // {
    //     float cos_theta = sampler.random() * 2 - 1;
    //     float sin_theta = sqrt(1 - pow(cos_theta, 2));
    //     float phi = sampler.random() * 2 * 3.14159;
    //     light_dir = vec3(sin_theta * cos(phi), cos_theta, sin_theta * sin(phi)).normalized();
    //     light_pos = -1e18f * light_dir;
    //     light_int = envmap_->pixelUniform(phi / 2 / 3.14159, cos_theta);
    //     return {light_pos, light_dir, light_int};
    // }
    // else
    // {
    auto hemisphereSampler = [&](const vec3 &normal) -> vec3
    {
        float r2 = sampler.random() * 0.999f;
        float phi = sampler.random() * 3.14159 * 2;
        float r = sqrt(r2);
        float h = sqrt(1 - r2);

        vec3 ax0 = abs(normal[0]) < 0.8 ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f);
        vec3 ax1 = ax0.cross(normal).normalized();
        vec3 ax2 = normal.cross(ax1).normalized();
        vec3 wi = h * normal + r * cos(phi) * ax1 + r * sin(phi) * ax2;
        return wi;
    };

    auto light_obj = sampleLight(sampler);
    auto [light_pos, light_bc1, light_bc2] = light_obj->sample(sampler);
    vec3 light_normal = light_obj->getNormal(light_bc1, light_bc2);
    vec3 light_uv = light_obj->getTexCoords(light_bc1, light_bc2);
    float light_pdf = p();
    vec3 light_dir = hemisphereSampler(light_normal);
    auto light_int = light_obj->mat->emission(light_dir, light_obj->getNormal(light_bc1, light_bc2));
    return {light_pos, light_dir, light_int / light_pdf};
    // }
}

float LightSampler::p()
{
    return 1.0 / (sum_weight_primitives_ + 1e-8);
}
