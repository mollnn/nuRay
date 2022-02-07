#ifndef Img_H
#define Img_H

#include "ImgConvert.h"

template <typename T>
class Img : public ImgConvert<T>
{
public:
    Img() {}
    Img(const Img &img) : ImgData<T>(img) {}
    Img(Img &&img) : ImgData<T>(img) {}
    Img(const ImgData<T> &img) : ImgData<T>(img) {}
    Img(ImgData<T> &&img) : ImgData<T>(img) {}
    Img(int width, int height, T range) : ImgData<T>(width, height, range) {}
    Img &operator=(const Img &img);
    Img &operator=(Img &&img);
    Img &operator=(const ImgData<T> &img);
    Img &operator=(ImgData<T> &&img);
};

//////////////////////////////////////////////

template <typename T>
Img<T> &Img<T>::operator=(const Img<T> &img)
{
    ImgData<T>::operator=(img);
    return *this;
}

template <typename T>
Img<T> &Img<T>::operator=(Img<T> &&img)
{
    ImgData<T>::operator=(std::move(img));
    return *this;
}

template <typename T>
Img<T> &Img<T>::operator=(const ImgData<T> &img)
{
    ImgData<T>::operator=(img);
    return *this;
}

template <typename T>
Img<T> &Img<T>::operator=(ImgData<T> &&img)
{
    ImgData<T>::operator=(std::move(img));
    return *this;
}

#endif // IMG_H
