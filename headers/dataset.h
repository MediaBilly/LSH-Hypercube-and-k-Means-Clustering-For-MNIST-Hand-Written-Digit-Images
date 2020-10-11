#pragma once

#include <string>
#include <vector>
#include "image.h"

class Dataset
{
    private:
        struct header
        {
            int magic_num;
            int num_of_images;
            int num_of_rows;
            int num_of_columns;
        };

        header head;
        std::vector<Image*> images;

    public:
        Dataset(std::string inputPath);
        int getImageDimension();
        std::vector<Image*> getImages();
        ~Dataset();
};

