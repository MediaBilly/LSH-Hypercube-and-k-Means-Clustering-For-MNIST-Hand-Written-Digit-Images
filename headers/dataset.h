#pragma once

#include <string>
#include <vector>
#include "image.h"

class Dataset
{
    private:
        struct header
        {
            unsigned int magic_num;
            unsigned int num_of_images;
            unsigned num_of_rows;
            unsigned int num_of_columns;
        };
        bool valid;
        header head;
        std::vector<Image*> images;

    public:
        Dataset(std::string inputPath);
        bool isValid();
        int getImageDimension();
        // Used to approximate good value of w
        int avg_NN_distance();
        std::vector<Image*> getImages();
        ~Dataset();
};

