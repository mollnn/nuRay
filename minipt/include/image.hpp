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

    void reallocate()
    {
        buffer.clear();
        buffer.resize(size_x);
        for (int i = 0; i < size_x; i++)
            buffer[i].resize(size_y);
    }

    Image(int sx = 0, int sy = 0) : size_x(sx), size_y(sy)
    {
        this->reallocate();
    }

    void Set(int x, int y, vec3 color)
    {
        assert(0 <= x && x < size_x);
        assert(0 <= y && y < size_y);
        buffer[x][y] = color;
    }

    vec3 Get(int x, int y)
    {
        assert(0 <= x && x < size_x);
        assert(0 <= y && y < size_y);
        return buffer[x][y];
    }

    vec3 Add(int x, int y, vec3 color)
    {
        // std::cerr << "Add " << x << " " << y << " " << color.x << " " << color.y << " " << color.z << std::endl;

        auto origin = this->Get(x, y);
        origin = origin + color;
        this->Set(x, y, origin);
        return origin;
    }

    TGAImage ToTGAImage()
    {
        TGAImage tga_image(size_x, size_y, 3);
        for (int i = 0; i < size_x; i++)
        {
            for (int j = 0; j < size_y; j++)
            {
                tga_image.set(i, j, TGAColor(buffer[i][j].x * 255, buffer[i][j].y * 255, buffer[i][j].z * 255));
            }
        }
        return tga_image;
    }

    void FromTGAImage(const TGAImage &tga_image)
    {
        size_x = tga_image.get_width();
        size_y = tga_image.get_height();
        reallocate();
        for (int i = 0; i < size_x; i++)
        {
            for (int j = 0; j < size_y; j++)
            {
                auto tga_color = tga_image.get(i, j);
                buffer[i][j].x = tga_color[2] / 255.;
                buffer[i][j].y = tga_color[1] / 255.;
                buffer[i][j].z = tga_color[0] / 255.;
            }
        }
    }

    void WriteToTGA(std::string filename)
    {
        ToTGAImage().write_tga_file(filename);
    }

    void ReadFromTGA(std::string filename)
    {
        TGAImage tga_image;
        tga_image.read_tga_file(filename);
        FromTGAImage(tga_image);
    }

    void Clamp()
    {
        for (int i = 0; i < size_x; i++)
        {
            for (int j = 0; j < size_y; j++)
            {
                buffer[i][j] = colorClamp(buffer[i][j]);
            }
        }
    }

    void FilpV()
    {
        for (int i = 0; i < size_x; i++)
        {
            for (int j = 0; j + j < size_y; j++)
            {
                swap(buffer[i][j], buffer[i][size_y - j]);
            }
        }
    }
};

#endif