#include "../material/matblinnphong.h"

MatBlinnPhong::MatBlinnPhong(const vec3 &Kd, const vec3 &Ks, float Ns) : Kd_(Kd), usetex_Kd_(false), Ks_(Ks), usetex_Ks_(false), Ns_(Ns) {}

vec3 MatBlinnPhong::sampleBxdf(Sampler &sampler, const vec3 &wo, const vec3 &normal) const
{
    float lambda = 0.5f;
    float r0 = sampler.random();

    vec3 ax0 = abs(normal[0]) < 0.8 ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f);
    vec3 ax1 = ax0.cross(normal).normalized();
    vec3 ax2 = normal.cross(ax1).normalized();

    if (r0 < lambda)
    {
        float r2 = sampler.random() * 0.99f;
        float phi = sampler.random() * 3.14159 * 2;
        float r = sqrt(r2);
        float h = sqrt(1 - r2);

        vec3 wi = h * normal + r * cos(phi) * ax1 + r * sin(phi) * ax2;

        return wi;
    }
    else
    {
        double r1 = sampler.random();
        double r2 = sampler.random();
        double cos_theta = pow(r1, 1.0f / (Ns_ + 2.0f));
        double sin_theta = sqrt(1 - cos_theta);
        double phi = 2.0f * 3.14159f * r2;
        vec3 h = cos_theta * normal + sin_theta * cos(phi) * ax1 + sin_theta * sin(phi) * ax2;
        vec3 wi = 2.0f * h.dot(wo) * h - wo;
        return wi;
    }
}

vec3 MatBlinnPhong::bxdf(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const
{
    if (wi.dot(normal) * wo.dot(normal) < 0)
        return 0.0f;
    if (wi.dot(normal) <= 1e-4 || wo.dot(normal) <= 1e-4f)
        return 0.0f;
    vec3 Rd = usetex_Kd_ ? map_Kd_.pixelUV(uv[0], uv[1]) : Kd_;
    vec3 Rs = usetex_Ks_ ? map_Ks_.pixelUV(uv[0], uv[1]) : Ks_;
    vec3 diffuse = Rd / 3.14159;
    vec3 h = (wo + wi).normalized();
    vec3 specular = (Ns_ + 2) / 3.14159 / 8 * Rs * pow(normal.dot(h), Ns_); // ! NEED FIX
    return diffuse + specular;
}

float MatBlinnPhong::pdf(const vec3 &wo, const vec3 &normal, const vec3 &wi) const
{
    if (wi.dot(normal) <= 1e-4 || wo.dot(normal) <= 1e-4f)
        return 1e-18f;
    float lambda = 0.5f;
    float pdf_diffuse = 1.0 / 3.14159 * (abs(wi.dot(normal)) + 1e-6f);
    vec3 h = (wo + wi).normalized();
    float pdf_specular = (Ns_ + 2) / 3.14159 / 8 * pow(abs(normal.dot(h)), Ns_ + 1);
    return lambda * pdf_diffuse + (1.0f - lambda) * pdf_specular;
}

bool MatBlinnPhong::isEmission() const
{
    return false;
}

bool MatBlinnPhong::isTransmission() const
{
    return false;
}

bool MatBlinnPhong::isSpecular(const vec3 &wo, const vec3 &normal, const vec3 &wi, const vec3 &uv) const
{
    if (Ns_ < 10)
        return false;
    vec3 Rd = usetex_Kd_ ? map_Kd_.pixelUV(uv[0], uv[1]) : Kd_;
    vec3 Rs = usetex_Ks_ ? map_Ks_.pixelUV(uv[0], uv[1]) : Ks_;
    float Id = Rd.norm();
    float Is = Rs.norm();
    float r = Is / (Is + Id);
    if (rand() * 1.0f / RAND_MAX < r)
        return true;
    return false;
}

bool MatBlinnPhong::requireLightSampling(const vec3 &wo, const vec3 &normal) const
{
    return true;
}

vec3 MatBlinnPhong::emission(const vec3 &wo, const vec3 &normal) const
{
    return vec3(0.0f, 0.0f, 0.0f);
}

float MatBlinnPhong::roughness(const vec3 &uv) const
{
    return 1.0f / Ns_;
}

vec3 MatBlinnPhong::reflectanceDiffuse(const vec3 &uv) const
{
    vec3 Rd = usetex_Kd_ ? map_Kd_.pixelUV(uv[0], uv[1]) : Kd_;
    return Rd;
}

vec3 MatBlinnPhong::reflectanceSpecular(const vec3 &uv) const
{
    vec3 Rs = usetex_Ks_ ? map_Ks_.pixelUV(uv[0], uv[1]) : Ks_;
    return Rs;
}