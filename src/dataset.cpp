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

    // Allocate space for images
    this->images = new std::vector<Image*>();

    // TODO:Read images
    for (int i = 0; i < this->head.num_of_images; i++) {
        Pixel img[this->head.num_of_columns* this->head.num_of_rows];

        input.read((char*)&img, sizeof(img));
        // Read pixels for every image
        //std::cout << "Image " << i+1 << ":\n";
        this->images->push_back(new Image(this->head.num_of_columns, this->head.num_of_rows));
        for (int p = 0; p < this->head.num_of_rows * this->head.num_of_columns; p++) {
            //input.read((char*)&pixel,sizeof(Pixel));
            this->images->at(i)->setPixel(p,img[p]);
        }
        //std::cout << "\n\n";
    }
    

    std::cout << "Magic Number: " << this->head.magic_num << std::endl;
    std::cout << "Number of images: " << this->head.num_of_images << std::endl;
    std::cout << "Number of rows: " << this->head.num_of_rows << std::endl;
    std::cout << "Number of columns: " << this->head.num_of_columns << std::endl;
    // Close dataset binary file
    input.close();
}

Dataset::~Dataset() {
    for (std::vector<Image*>::iterator it = this->images->begin();it < this->images->end();it++) {
        delete *it;
    }
    delete this->images;
}