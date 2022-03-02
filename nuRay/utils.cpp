#include "utils.h"

void Utils::testEnergyConversion()
{
    // MatGGXRefl mbp(1.0f, 0.5f);
    // float ans = 0;
    // float sum_pdf = 0;
    // float min_pdf = 1e9;
    // float max_val = 0;
    // for (int i = 0; i < 1000000; i++)
    // {
    //     vec3 n(0.0f, 1.0f, 0.0f);
    //     vec3 wo(0.0f, 1.0f, 0.0f);
    //     vec3 wi = mbp.sampleBxdf(wo, n);
    //     float pdf = mbp.pdf(wo, n, wi);

    //     sum_pdf += pdf;
    //     min_pdf = std::min(min_pdf, pdf);
    //     vec3 bxdf = mbp.bxdf(wo, n, wi, {0.0f, 0.0f, 0.0f});
    //     ans += bxdf[0] / pdf * n.dot(wi);
    //     max_val = std::max(max_val, abs(bxdf[0]));
    // }
    // qDebug() << ans / 1e6 << sum_pdf / 1e6 << min_pdf << max_val;

    MatGGX mat(0.5, 0.5, 1.1);

    float ans = 0;
    for (int i = 0; i < 10000; i++)
    {
        vec3 n(0.0f, 1.0f, 0.0f);
        vec3 wo(0.6f, 0.8f, 0.0f);
        n=n.normalized();
        vec3 wi = mat.sampleBxdf(wo, n);
        float pdf = mat.pdf(wo, n, wi);
        ans += 1.0f / pdf;
    }
    std::cout << "Self test " << ans / 10000 << std::endl;
}