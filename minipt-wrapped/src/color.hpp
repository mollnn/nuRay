#ifndef _COLOR_HPP_
#define _COLOR_HPP_

#include <bits/stdc++.h>
#include "vec3.hpp"

double fClamp(double x);
vec3 colorClamp(vec3 c);
double fDeGamma(double x, double gamma = 2.2);
vec3 colorDeGamma(vec3 c, double gamma = 2.2);
double fEnGamma(double x, double gamma = 2.2);
vec3 colorEnGamma(vec3 c, double gamma = 2.2);
uint8_t fFloatToUint8(double f);
std::tuple<uint8_t, uint8_t, uint8_t> colorFloatToUint8(vec3 c);
double fUint8ToFloat(uint8_t f);
vec3 colorUint8ToFloat(uint8_t r, uint8_t g, uint8_t b);

#endif