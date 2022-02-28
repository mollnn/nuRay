#include "matggx.h"

MatGGX::MatGGX(const vec3 &Kd, float alpha, float ior) : Kd_(Kd), usetex_Kd_(false), alpha_(alpha), ior_(ior) {}

vec3 MatGGX::sampleBxdf(const vec3 &wo, const vec3 &normal) const
{
    // Naive
    float r2 = rand() * 1.0 / RAND_MAX * 0.99f;
    float phi = rand() * 1.0 / RAND_MAX * 3.14159 * 2;
    float r = sqrt(r2);
    float h = sqrt(1 - r2);

    vec3 ax0 = abs(normal[0]) < 0.8 ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f);
    vec3 ax1 = ax0.cross(normal).normalized();
    vec3 ax2 = normal.cross(ax1).normalized();
    vec3 wi = h * normal + r * cos(phi) * ax1 + r * sin(phi) * ax2;
    if (rand() % 2)
        wi = -wi;

    return wi;
}

vec3 MatGGX::F(const vec3 &v, const vec3 &m, const vec3 &Kd) const
{
    return Kd + (vec3(1.0f) - Kd) * pow(1 - v.dot(m), 5);
}

float MatGGX::G1(const vec3 &v, const vec3 &m, const vec3 &n) const
{
    float vm = v.dot(m);
    float vn = v.dot(n);
    if (vm * vn < 0)
        return 0.0f;
    float r2 = 1.0f + pow(alpha_ * tan(acos(vm)), 2);
    return 2.0f / (1.0f + sqrt(r2));
}

float MatGGX::G(const vec3 &i, const vec3 &o, const vec3 &m, const vec3 &n) const
{
    return G1(i, m, n) * G1(o, m, n);
}

float MatGGX::D(const vec3 &m, const vec3 &n) const
{
    if (n.dot(m) < 0)
        return 0.0f;
    return pow(alpha_, 2) / 3.14159f / pow(pow(n.dot(m), 2) * (pow(alpha_, 2) - 1) + 1, 2);
}

vec3 MatGGX::fr(const vec3 &i, const vec3 &o, const vec3 &n, const vec3 &Kd) const
{
    vec3 h = (i + o).normalized();
    vec3 nn = i.dot(n) > 0 ? n : -n;
    vec3 f = F(i, h, Kd);
    vec3 g = G(i, o, h, nn);
    vec3 d = D(h, nn);
    float denom = 4.0f * i.dot(nn) * o.dot(nn);
    return f * g * d / (denom + 1e-6f);
}

vec3 MatGGX::ft(const vec3 &i, const vec3 &o, const vec3 &n, const vec3 &Kd) const
{
    float eta = i.dot(n) > 0 ? eta : 1.0f / eta; // eta_o / eta_i
    vec3 h = -(i + eta * o).normalized();
    float A = abs(i.dot(h) * o.dot(h) / i.dot(n) / o.dot(n));
    vec3 nn = i.dot(n) > 0 ? n : -n;
    vec3 f = F(i, h, Kd);
    vec3 g = G(i, o, h, nn);
    vec3 d = D(h, nn);
    vec3 numer = pow(eta, 2) * (vec3(1.0f) - f) * g * h;
    float denom = pow(i.dot(h) + eta * o.dot(h), 2);
    return A * numer / (denom + 1e-6f);
}

vec3 MatGGX::bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const
{
    vec3 Kd = Kd_;
    if (usetex_Kd_)
    {
        Kd = map_Kd_.pixelUV(uv[0], uv[1]);
    }

    float bi = wi.dot(normal);
    float bo = wo.dot(normal);
    if (bi * bo > 0)
        return fr(wi, wo, normal, Kd);
    else
        return ft(wi, wo, normal, Kd);
}

float MatGGX::pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const
{
    // Naive
    return 1.0 / 3.14159 * (abs(wi.dot(normal)) + 1e-4f) / 2.0;

    // if (wi.dot(normal) <= 1e-4 || wo.dot(normal) <= 1e-4f)
    //     return 1e18f;
    // vec3 wh = (wo + wi).normalized();
    // float D = pow(alpha_, 2) / 3.14159f / pow(pow(normal.dot(wh), 2) * (pow(alpha_, 2) - 1) + 1, 2);
    // return D * normal.dot(wh) / 4 + 1e-8f;
}

bool MatGGX::isEmission() const
{
    return false;
}

bool MatGGX::isTransmission() const
{
    return true;
}

bool MatGGX::requireLightSampling(const vec3 &wo, const vec3 &normal) const
{
    return true;
}

vec3 MatGGX::emission(const vec3 &wo, const vec3 &normal) const
{
    return vec3(0.0f, 0.0f, 0.0f);
}