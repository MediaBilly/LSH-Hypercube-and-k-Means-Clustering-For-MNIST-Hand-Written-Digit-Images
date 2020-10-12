#include "../headers/dataset.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cerrno>

#define SWAP_INT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

Dataset::Dataset(std::string inputPath)
{
    // Open dataset binary file
    std::ifstream input(inputPath, std::ios::out | std::ios::binary);
    if(!input) {
        std::cout << "Cannot open file!:" << strerror(errno) << std::endl;
        return;
    }
    // Read header
    input.read((char*)&this->head,sizeof(header));
    
    // Swap endianess
    this->head.magic_num = SWAP_INT32(this->head.magic_num);
    this->head.num_of_images = SWAP_INT32(this->head.num_of_images);
    this->head.num_of_rows = SWAP_INT32(this->head.num_of_rows);
    this->head.num_of_columns = SWAP_INT32(this->head.num_of_columns);

    // TODO:Read images
    for (int i = 0; i < this->head.num_of_images; i++) {
        Pixel img[getImageDimension()];

        input.read((char*)&img, sizeof(img));
        // Read pixels for every image
        this->images.push_back(new Image(i+1, this->head.num_of_columns, this->head.num_of_rows));
        for (int p = 0; p < this->getImageDimension(); p++) {
            this->images.at(i)->setPixel(p,img[p]);
        }
    }
    // Close dataset binary file
    input.close();
}

int Dataset::getImageDimension() {
    return this->head.num_of_rows * this->head.num_of_columns;
}

std::vector<Image*> Dataset::getImages(){
    return this->images;
}

Dataset::~Dataset() {
    for (std::vector<Image*>::iterator it = this->images.begin();it < this->images.end();it++) {
        delete *it;
    }
}