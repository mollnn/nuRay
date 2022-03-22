#include "../utils/utils.h"

void Utils::testEnergyConversion()
{
}

float Utils::mse(const QImage &a, const QImage &b)
{
    int w = std::min(a.width(), b.width());
    int h = std::min(a.height(), b.height());
    float mse = 0.0f;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            auto p = a.pixelColor(j, i);
            auto q = b.pixelColor(j, i);
            float dx = (p.redF() - q.redF());
            float dy = (p.greenF() - q.greenF());
            float dz = (p.blueF() - q.blueF());
            mse += dx * dx + dy * dy + dz * dz;
        }
    }
    return mse / w / h;
}