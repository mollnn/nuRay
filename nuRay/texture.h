#ifndef __TEXTURE_H_
#define __TEXTURE_H_

#include <bits/stdc++.h>
#include "Img.h"

class Texture 
{
private:
    Img<float> img;
public:
    Texture() = default;
    Texture(const std::string& filename);
    void load(const std::string& filename);
    void save(const std::string& filename);
    float& pixel(int x,int y);
    float pixelBi(float x,float y);
    float pixelUV(float x,float y);
};

#endif