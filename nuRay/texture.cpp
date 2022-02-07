#include "texture.h"

// TODO: directly reimplement these with QImage, since MyImg is only for grayscale image

void Texture::load(const std::string& filename)
{
    QImage q;
    Img<uint8_t> tmp;
    tmp.fromQImage(QImage(QString::fromStdString(filename)));
    img = tmp.cast<float>() / 255.0;
}

Texture::Texture(const std::string& filename)
{
    load(filename);
}

void Texture::save(const std::string& filename)
{
    // TODO
}

float& Texture::pixel(int x,int y)
{
    return img.pixel(x,y);
}

float Texture::pixelBi(float x,float y)
{
    return img.pixelBi(x,y);
}

float Texture::pixelUV(float x,float y)
{
    return img.pixelBi(x*img.width(),y*img.height());
}