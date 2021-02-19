// This file is not written myself !

#ifndef __TGAIMAGE_H__
#define __TGAIMAGE_H__

#include <cstdint>
#include <fstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>

#include "vec3.hpp"

#pragma pack(push, 1)
struct TGA_Header
{
    std::uint8_t idlength{};
    std::uint8_t colormaptype{};
    std::uint8_t datatypecode{};
    std::uint16_t colormaporigin{};
    std::uint16_t colormaplength{};
    std::uint8_t colormapdepth{};
    std::uint16_t x_origin{};
    std::uint16_t y_origin{};
    std::uint16_t width{};
    std::uint16_t height{};
    std::uint8_t bitsperpixel{};
    std::uint8_t imagedescriptor{};
};
#pragma pack(pop)

struct TGAColor
{
    std::uint8_t bgra[4] = {0, 0, 0, 0};
    std::uint8_t bytespp = {0};

    TGAColor() = default;
    TGAColor(const std::uint8_t R, const std::uint8_t G, const std::uint8_t B, const std::uint8_t A = 255) : bgra{B, G, R, A}, bytespp(4) {}
    TGAColor(const std::uint8_t v) : bgra{v, 0, 0, 0}, bytespp(1) {}

    TGAColor(const std::uint8_t *p, const std::uint8_t bpp) : bgra{0, 0, 0, 0}, bytespp(bpp)
    {
        for (int i = 0; i < bpp; i++)
            bgra[i] = p[i];
    }

    std::uint8_t &operator[](const int i) { return bgra[i]; }

    TGAColor operator*(const double intensity) const
    {
        TGAColor res = *this;
        double clamped = std::max(0., std::min(intensity, 1.));
        for (int i = 0; i < 4; i++)
            res.bgra[i] = bgra[i] * clamped;
        return res;
    }

    TGAColor(const vec3 &color) : bgra{color.z * 255, color.y * 255, color.x * 255, 0}
    {
    }
};

class TGAImage
{
protected:
    std::vector<std::uint8_t> data;
    int width;
    int height;
    int bytespp;

    bool load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out) const;

public:
    enum Format
    {
        GRAYSCALE = 1,
        RGB = 3,
        RGBA = 4
    };

    TGAImage();
    TGAImage(const int w, const int h, const int bpp);
    bool read_tga_file(const std::string filename);
    bool write_tga_file(const std::string filename, const bool vflip = true, const bool rle = true) const;
    void flip_horizontally();
    void flip_vertically();
    void scale(const int w, const int h);
    TGAColor get(const int x, const int y) const;
    void set(const int x, const int y, const TGAColor &c);
    int get_width() const;
    int get_height() const;
    int get_bytespp();
    std::uint8_t *buffer();
    void clear();
};

TGAImage::TGAImage() : data(), width(0), height(0), bytespp(0) {}
TGAImage::TGAImage(const int w, const int h, const int bpp) : data(w * h * bpp, 0), width(w), height(h), bytespp(bpp) {}

bool TGAImage::read_tga_file(const std::string filename)
{
    std::ifstream in;
    in.open(filename, std::ios::binary);
    if (!in.is_open())
    {
        std::cerr << "can't open file " << filename << "\n";
        in.close();
        return false;
    }
    TGA_Header header;
    in.read(reinterpret_cast<char *>(&header), sizeof(header));
    if (!in.good())
    {
        in.close();
        std::cerr << "an error occured while reading the header\n";
        return false;
    }
    width = header.width;
    height = header.height;
    bytespp = header.bitsperpixel >> 3;
    if (width <= 0 || height <= 0 || (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA))
    {
        in.close();
        std::cerr << "bad bpp (or width/height) value\n";
        return false;
    }
    size_t nbytes = bytespp * width * height;
    data = std::vector<std::uint8_t>(nbytes, 0);
    if (3 == header.datatypecode || 2 == header.datatypecode)
    {
        in.read(reinterpret_cast<char *>(data.data()), nbytes);
        if (!in.good())
        {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    }
    else if (10 == header.datatypecode || 11 == header.datatypecode)
    {
        if (!load_rle_data(in))
        {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    }
    else
    {
        in.close();
        std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
        return false;
    }
    if (!(header.imagedescriptor & 0x20))
        flip_vertically();
    if (header.imagedescriptor & 0x10)
        flip_horizontally();
    std::cerr << width << "x" << height << "/" << bytespp * 8 << "\n";
    in.close();
    return true;
}

bool TGAImage::load_rle_data(std::ifstream &in)
{
    size_t pixelcount = width * height;
    size_t currentpixel = 0;
    size_t currentbyte = 0;
    TGAColor colorbuffer;
    do
    {
        std::uint8_t chunkheader = 0;
        chunkheader = in.get();
        if (!in.good())
        {
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
        if (chunkheader < 128)
        {
            chunkheader++;
            for (int i = 0; i < chunkheader; i++)
            {
                in.read(reinterpret_cast<char *>(colorbuffer.bgra), bytespp);
                if (!in.good())
                {
                    std::cerr << "an error occured while reading the header\n";
                    return false;
                }
                for (int t = 0; t < bytespp; t++)
                    data[currentbyte++] = colorbuffer.bgra[t];
                currentpixel++;
                if (currentpixel > pixelcount)
                {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
        else
        {
            chunkheader -= 127;
            in.read(reinterpret_cast<char *>(colorbuffer.bgra), bytespp);
            if (!in.good())
            {
                std::cerr << "an error occured while reading the header\n";
                return false;
            }
            for (int i = 0; i < chunkheader; i++)
            {
                for (int t = 0; t < bytespp; t++)
                    data[currentbyte++] = colorbuffer.bgra[t];
                currentpixel++;
                if (currentpixel > pixelcount)
                {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
    } while (currentpixel < pixelcount);
    return true;
}

bool TGAImage::write_tga_file(const std::string filename, const bool vflip, const bool rle) const
{
    std::uint8_t developer_area_ref[4] = {0, 0, 0, 0};
    std::uint8_t extension_area_ref[4] = {0, 0, 0, 0};
    std::uint8_t footer[18] = {'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'};
    std::ofstream out;
    out.open(filename, std::ios::binary);
    if (!out.is_open())
    {
        std::cerr << "can't open file " << filename << "\n";
        out.close();
        return false;
    }
    TGA_Header header;
    header.bitsperpixel = bytespp << 3;
    header.width = width;
    header.height = height;
    header.datatypecode = (bytespp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header.imagedescriptor = vflip ? 0x00 : 0x20; // top-left or bottom-left origin
    out.write(reinterpret_cast<const char *>(&header), sizeof(header));
    if (!out.good())
    {
        out.close();
        std::cerr << "can't dump the tga file\n";
        return false;
    }
    if (!rle)
    {
        out.write(reinterpret_cast<const char *>(data.data()), width * height * bytespp);
        if (!out.good())
        {
            std::cerr << "can't unload raw data\n";
            out.close();
            return false;
        }
    }
    else
    {
        if (!unload_rle_data(out))
        {
            out.close();
            std::cerr << "can't unload rle data\n";
            return false;
        }
    }
    out.write(reinterpret_cast<const char *>(developer_area_ref), sizeof(developer_area_ref));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write(reinterpret_cast<const char *>(extension_area_ref), sizeof(extension_area_ref));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write(reinterpret_cast<const char *>(footer), sizeof(footer));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.close();
    return true;
}

// TODO: it is not necessary to break a raw chunk for two equal pixels (for the matter of the resulting size)
bool TGAImage::unload_rle_data(std::ofstream &out) const
{
    const std::uint8_t max_chunk_length = 128;
    size_t npixels = width * height;
    size_t curpix = 0;
    while (curpix < npixels)
    {
        size_t chunkstart = curpix * bytespp;
        size_t curbyte = curpix * bytespp;
        std::uint8_t run_length = 1;
        bool raw = true;
        while (curpix + run_length < npixels && run_length < max_chunk_length)
        {
            bool succ_eq = true;
            for (int t = 0; succ_eq && t < bytespp; t++)
                succ_eq = (data[curbyte + t] == data[curbyte + t + bytespp]);
            curbyte += bytespp;
            if (1 == run_length)
                raw = !succ_eq;
            if (raw && succ_eq)
            {
                run_length--;
                break;
            }
            if (!raw && !succ_eq)
                break;
            run_length++;
        }
        curpix += run_length;
        out.put(raw ? run_length - 1 : run_length + 127);
        if (!out.good())
        {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
        out.write(reinterpret_cast<const char *>(data.data() + chunkstart), (raw ? run_length * bytespp : bytespp));
        if (!out.good())
        {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
    }
    return true;
}

TGAColor TGAImage::get(const int x, const int y) const
{
    if (!data.size() || x < 0 || y < 0 || x >= width || y >= height)
        return {};
    return TGAColor(data.data() + (x + y * width) * bytespp, bytespp);
}

void TGAImage::set(int x, int y, const TGAColor &c)
{
    if (!data.size() || x < 0 || y < 0 || x >= width || y >= height)
        return;
    memcpy(data.data() + (x + y * width) * bytespp, c.bgra, bytespp);
}

int TGAImage::get_bytespp()
{
    return bytespp;
}

int TGAImage::get_width() const
{
    return width;
}

int TGAImage::get_height() const
{
    return height;
}

void TGAImage::flip_horizontally()
{
    if (!data.size())
        return;
    int half = width >> 1;
    for (int i = 0; i < half; i++)
    {
        for (int j = 0; j < height; j++)
        {
            TGAColor c1 = get(i, j);
            TGAColor c2 = get(width - 1 - i, j);
            set(i, j, c2);
            set(width - 1 - i, j, c1);
        }
    }
}

void TGAImage::flip_vertically()
{
    if (!data.size())
        return;
    size_t bytes_per_line = width * bytespp;
    std::vector<std::uint8_t> line(bytes_per_line, 0);
    int half = height >> 1;
    for (int j = 0; j < half; j++)
    {
        size_t l1 = j * bytes_per_line;
        size_t l2 = (height - 1 - j) * bytes_per_line;
        std::copy(data.begin() + l1, data.begin() + l1 + bytes_per_line, line.begin());
        std::copy(data.begin() + l2, data.begin() + l2 + bytes_per_line, data.begin() + l1);
        std::copy(line.begin(), line.end(), data.begin() + l2);
    }
}

std::uint8_t *TGAImage::buffer()
{
    return data.data();
}

void TGAImage::clear()
{
    data = std::vector<std::uint8_t>(width * height * bytespp, 0);
}

void TGAImage::scale(int w, int h)
{
    if (w <= 0 || h <= 0 || !data.size())
        return;
    std::vector<std::uint8_t> tdata(w * h * bytespp, 0);
    int nscanline = 0;
    int oscanline = 0;
    int erry = 0;
    size_t nlinebytes = w * bytespp;
    size_t olinebytes = width * bytespp;
    for (int j = 0; j < height; j++)
    {
        int errx = width - w;
        int nx = -bytespp;
        int ox = -bytespp;
        for (int i = 0; i < width; i++)
        {
            ox += bytespp;
            errx += w;
            while (errx >= (int)width)
            {
                errx -= width;
                nx += bytespp;
                memcpy(tdata.data() + nscanline + nx, data.data() + oscanline + ox, bytespp);
            }
        }
        erry += h;
        oscanline += olinebytes;
        while (erry >= (int)height)
        {
            if (erry >= (int)height << 1) // it means we jump over a scanline
                memcpy(tdata.data() + nscanline + nlinebytes, tdata.data() + nscanline, nlinebytes);
            erry -= height;
            nscanline += nlinebytes;
        }
    }
    data = tdata;
    width = w;
    height = h;
}

#endif //__IMAGE_H__