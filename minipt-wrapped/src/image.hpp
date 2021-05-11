#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <vector>
#include "color.hpp"
#include "tgaimage.hpp"

#include "vec3.hpp"

struct Image
{
    int size_x;
    int size_y;

    std::vector<std::vector<vec3>> buffer;

    void reallocate();

    Image(int sx = 0, int sy = 0);

    void Set(int x, int y, vec3 color);

    vec3 Get(int x, int y);

    vec3 Add(int x, int y, vec3 color);

    TGAImage ToTGAImage();

    void FromTGAImage(const TGAImage &tga_image);

    void WriteToTGA(std::string filename);

    void ReadFromTGA(std::string filename);

    void Clamp();

    void FilpV();
};

#endif