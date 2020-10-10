#pragma once

typedef unsigned char Pixel;

class Image
{
    private:
        int width;
        int height;
        Pixel *pixels;
    public:
        Image(int width, int height);
        bool setPixel(int index, Pixel pixel);
        Pixel getPixel(int index);
        ~Image();
};
