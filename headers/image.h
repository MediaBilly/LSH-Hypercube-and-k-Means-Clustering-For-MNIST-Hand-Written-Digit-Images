#pragma once

typedef unsigned char Pixel;

class Image
{
    private:
        int id;
        int width;
        int height;
        Pixel *pixels;
    public:
        Image(int id, int width, int height);
        bool setPixel(int index, Pixel pixel);
        Pixel getPixel(int index);
        int getSize();
        // Calculates the p-norm distance to another image
        double distance(Image *image, int norm);
        ~Image();
};
