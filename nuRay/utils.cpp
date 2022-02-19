#include "utils.h"

void Utils::testEnergyConversion()
{
    MatGGXRefl mbp(1.0f, 0.5f);
    float ans = 0;
    for (int i = 0; i < 1000000; i++)
    {
        vec3 n(0.0f, 1.0f, 0.0f);
        vec3 wo(0.0f, 1.0f, 0.0f);
        vec3 wi = mbp.sampleBxdf(wo, n);
        float pdf = mbp.pdf(wo, n, wi);
        vec3 bxdf = mbp.bxdf(wo, n, wi, {0.0f, 0.0f, 0.0f});
        ans += bxdf[0] / pdf * n.dot(wi);
    }
    qDebug() << ans / 1e6;
}