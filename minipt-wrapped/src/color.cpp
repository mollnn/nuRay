#include "color.hpp"

double fClamp(double x)
{
    x = std::min(x, 1.0);
    x = std::max(x, 0.0);
    return x;
}

vec3 colorClamp(vec3 c)
{
    c.x = fClamp(c.x);
    c.y = fClamp(c.y);
    c.z = fClamp(c.z);
    return c;
}

double fDeGamma(double x, double gamma)
{
    return pow(x, 1.0 / gamma);
}

vec3 colorDeGamma(vec3 c, double gamma)
{
    c.x = fDeGamma(c.x, gamma);
    c.y = fDeGamma(c.y, gamma);
    c.z = fDeGamma(c.z, gamma);
    return c;
}

double fEnGamma(double x, double gamma)
{
    return pow(x, gamma);
}

vec3 colorEnGamma(vec3 c, double gamma)
{
    c.x = fEnGamma(c.x, gamma);
    c.y = fEnGamma(c.y, gamma);
    c.z = fEnGamma(c.z, gamma);
    return c;
}

uint8_t fFloatToUint8(double f)
{
    return 255 * (f + 1e-6);
}

std::tuple<uint8_t, uint8_t, uint8_t> colorFloatToUint8(vec3 c)
{
    return make_tuple(fFloatToUint8(c.x), fFloatToUint8(c.y), fFloatToUint8(c.z));
}

double fUint8ToFloat(uint8_t f)
{
    return f * 1.0 / 255;
}

vec3 colorUint8ToFloat(uint8_t r, uint8_t g, uint8_t b)
{
    return {fUint8ToFloat(r), fUint8ToFloat(g), fUint8ToFloat(b)};
}