#include "../headers/dataset.h"
#include "../headers/bruteforce_search.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cerrno>

#define SWAP_INT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
#define W_SAMPLE_SIZE 50

Dataset::Dataset(std::string inputPath)
{   
    // Open dataset binary file
    std::ifstream input(inputPath, std::ios::out | std::ios::binary);
    if(!input) {
        std::cout << "Cannot open file!:" << strerror(errno) << std::endl;
        this->valid = false;
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
        this->images.push_back(new Image(i, this->head.num_of_columns, this->head.num_of_rows));
        for (int p = 0; p < this->getImageDimension(); p++) {
            this->images.at(i)->setPixel(p,img[p]);
        }
    }
    // Close dataset binary file
    input.close();

    this->valid = true;
}

bool Dataset::isValid() {
    return this->valid;
}

int Dataset::getImageDimension() {
    return this->head.num_of_rows * this->head.num_of_columns;
}


int Dataset::avg_NN_distance() {
    int step = images.size() / W_SAMPLE_SIZE;
    double dist_sum = 0.0;
    Bruteforce_Search bf(images);

    for(unsigned int i = 0; i < images.size(); i += step) {
        dist_sum += bf.exactNN(images[i], 2)[1].first;
    }

    return dist_sum / W_SAMPLE_SIZE;
}


std::vector<Image*> Dataset::getImages(){
    return this->images;
}

Dataset::~Dataset() {
    for (std::vector<Image*>::iterator it = this->images.begin();it < this->images.end();it++) {
        delete *it;
    }
}