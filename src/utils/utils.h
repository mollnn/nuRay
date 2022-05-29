#ifndef _UTILS_H_
#define _UTILS_H_

#include "../material/matlambert.h"
#include <QApplication>
#include <QDebug>
#include <QImage>

class Utils
{
public:
    void testEnergyConversion();
    float mse(const QImage& a, const QImage& b);
};

#endif