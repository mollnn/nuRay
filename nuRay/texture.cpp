#include "texture.h"
#include <QImage>
#include <QDebug>

void Texture::load(const std::string &filename)
{
    QImage image_src = QImage(QString::fromStdString(filename).replace("\\", "/")).convertToFormat(QImage::Format_RGBA8888);
    width_ = image_src.width();
    height_ = image_src.height();
    img_ = new vec3[width_ * height_];
#pragma omp parallel for
    for (int i = 0; i < height_; i++)
    {
        for (int j = 0; j < width_; j++)
        {
            QColor color = image_src.pixelColor(j, i);
            // * Gamma correction
            img_[(i * width_ + j)][0] = std::pow(color.redF(), 2.2f);
            img_[(i * width_ + j)][1] = std::pow(color.greenF(), 2.2f);
            img_[(i * width_ + j)][2] = std::pow(color.blueF(), 2.2f);
        }
        // // ! Fast but maybe unsafe
        // uchar *ptr = image_src.scanLine(i);
        // for (int j = 0; j < width_; j++)
        // {
        //     img_[(i * width_ + j)][0] = std::pow(ptr[j * 4 + 0] / 255.0f, 2.2f);
        //     img_[(i * width_ + j)][1] = std::pow(ptr[j * 4 + 1] / 255.0f, 2.2f);
        //     img_[(i * width_ + j)][2] = std::pow(ptr[j * 4 + 2] / 255.0f, 2.2f);
        // }
    }
}

Texture::Texture(const std::string &filename)
{
    load(filename);
}

Texture::Texture(int width, int height) : width_(width), height_(height), img_(new vec3[width * height])
{
}

void Texture::save(const std::string &filename)
{
    // TODO
}

vec3 &Texture::pixel(int x, int y)
{
    if (x < 0 || y < 0 || x >= width_ || y >= height_)
        throw("out of bound");
    return img_[y * width_ + x];
}

vec3 Texture::pixel(int x, int y) const
{
    if (x < 0 || y < 0 || x >= width_ || y >= height_)
        return vec3(0, 0, 0);
    return img_[y * width_ + x];
}

vec3 Texture::pixelBi(float x, float y) const
{
    int x0 = floor(x), y0 = floor(y);
    int x1 = x0 + 1, y1 = y0 + 1;
    return pixel(x1, y1) * (x - x0) * (y - y0) + pixel(x1, y0) * (x - x0) * (y1 - y) +
           pixel(x0, y1) * (x1 - x) * (y - y0) + pixel(x0, y0) * (x1 - x) * (y1 - y);
}

vec3 Texture::pixelUV(float x, float y) const
{
    return pixelBi(fmod(fmod(x, 1.0f) + 1.0f, 1.0f) * width_, fmod(fmod((1.0f - y), 1.0f) + 1.0f, 1.0f) * height_);
}

int Texture::width() const
{
    return width_;
}