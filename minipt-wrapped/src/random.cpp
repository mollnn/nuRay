#include "random.hpp"

int STDRANDOM()
{
    return rand()*rand();
}

uint32_t xorshf96()
{
    static uint32_t x = STDRANDOM(), y = STDRANDOM(), z = STDRANDOM();
    uint32_t t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;
    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;
    return z;
}

// 随机数生成器
double RAND(unsigned short seed[3])
{
    return (double)xorshf96() / (double)0xffffffffU;
}

double RAND()
{
    return (double)xorshf96() / (double)0xffffffffU;
}

double randf()
{
    return (double)xorshf96() / (double)0xffffffffU;
}