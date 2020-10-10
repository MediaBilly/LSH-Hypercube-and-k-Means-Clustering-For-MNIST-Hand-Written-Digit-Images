#include "../headers/image.h"

Image::Image(int width,int height) {
    this->width = width;
    this->height = height;
    this->pixels = new Pixel[width*height];
}

bool Image::setPixel(int index,Pixel pixel) {
    // Check bounds
    if (index >= this->width*this->height || index < 0) {
        return false;
    }
    
    // Set pixel color
    this->pixels[index] = pixel;
    return true;
}

Pixel Image::getPixel(int index) {
    return (index < this->width*this->height && index >= 0) ? this->pixels[index] : -1;
}

Image::~Image() {
    delete[] this->pixels;
}