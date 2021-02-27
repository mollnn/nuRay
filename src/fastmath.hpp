#ifndef _FASTMATH_HPP_
#define _FASTMATH_HPP_

#include <bits/stdc++.h>
using namespace std;

namespace fastmath
{
    const double pi = acos(-1);

    const int siz_cos_l = 1e4;
    const double lim_cos_l = 2 * pi;
    const double step_cos_l = lim_cos_l / siz_cos_l;

    vector<double> mem_cos_l(siz_cos_l + 2);

    double get_cos_l(double x)
    {
        if (x < 0 || x > 2 * pi)
            x = fmod(x, 2 * pi);
        int idx = (x + 1e-7) / step_cos_l;
        return mem_cos_l[idx];
    }

    double get_cos_l_(double x)
    {
        if (x < 0 || x > 2 * pi)
            x = fmod(x, 2 * pi);
        int idx1k = 1e3 * (x + 1e-7) / step_cos_l;
        int idx = idx1k / 1000;
        int offset = idx1k - idx * 1000;
        return (mem_cos_l[idx] * offset + mem_cos_l[idx + 1] * (1000 - offset)) / 1000;
    }

    void presolve()
    {
        cout << "Math Presolving..." << endl;
        for (int i = 0; i <= siz_cos_l; i++)
        {
            mem_cos_l[i] = cos(lim_cos_l / siz_cos_l * i);
        }
        cout << "Math Finish!" << endl;
    }
};

#endif