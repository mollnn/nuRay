#include "matggxrefl.h"

MatGGXRefl::MatGGXRefl(const vec3 &Kd, float alpha) : Kd_(Kd), usetex_Kd_(false), alpha_(alpha) {}

vec3 MatGGXRefl::sampleBxdf(const vec3 &wo, const vec3 &normal) const
{
    float r2 = rand() * 1.0 / RAND_MAX * 0.99f;
    float phi = rand() * 1.0 / RAND_MAX * 3.14159 * 2;
    float r = sqrt(r2);
    float h = sqrt(1 - r2);

    vec3 ax0 = abs(normal[0]) < 0.8 ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f);
    vec3 ax1 = ax0.cross(normal).normalized();
    vec3 ax2 = normal.cross(ax1).normalized();
    vec3 wi = h * normal + r * cos(phi) * ax1 + r * sin(phi) * ax2;

    return wi;
}

vec3 MatGGXRefl::bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const
{
    vec3 Kd = Kd_;
    if (usetex_Kd_)
    {
        Kd = map_Kd_.pixelUV(uv[0], uv[1]);
    }

    vec3 wh = (wo + wi).normalized();
    vec3 D = pow(alpha_, 2) / 3.14159f / pow(pow(normal.dot(wh), 2) * (pow(alpha_, 2) - 1) + 1, 2);
    vec3 Gl = 2 * normal.dot(wi) / (normal.dot(wi) + sqrt(pow(alpha_ * 0.5 + 0.5, 2) + (1 - pow(alpha_ * 0.5 + 0.5, 2) * pow(normal.dot(wi), 2))));
    vec3 Gv = 2 * normal.dot(wo) / (normal.dot(wo) + sqrt(pow(alpha_ * 0.5 + 0.5, 2) + (1 - pow(alpha_ * 0.5 + 0.5, 2) * pow(normal.dot(wo), 2))));
    vec3 F = Kd + (vec3(1.0f) - Kd) * pow(1.0f - normal.dot(wi), 5);
    vec3 f = D * Gl * Gv * F / 4 / normal.dot(wo) / normal.dot(wi);
    return f;
}

float MatGGXRefl::pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const
{
    return 1.0 / 3.14159 * (wi.dot(normal) + 1e-4f);
}

bool MatGGXRefl::isEmission() const
{
    return false;
}

bool MatGGXRefl::requireLightSampling(const vec3 &wo, const vec3 &normal) const
{
    return true;
}

vec3 MatGGXRefl::emission(const vec3 &wo, const vec3 &normal) const
{
    return vec3(0.0f, 0.0f, 0.0f);
}