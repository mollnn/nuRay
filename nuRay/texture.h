#ifndef __TEXTURE_H_
#define __TEXTURE_H_

#include <bits/stdc++.h>
#include "vec3.h"

class Texture
{
private:
    int width_, height_;
    vec3 *img_;

public:
    Texture() = default;
    Texture(const std::string &filename);
    Texture(int width, int height);
    void load(const std::string &filename);
    void save(const std::string &filename);
    vec3 &pixel(int x, int y);
    vec3 pixel(int x, int y) const;
    vec3 pixelBi(float x, float y) const;
    vec3 pixelUV(float x, float y) const;
    int width() const;
};

#endif