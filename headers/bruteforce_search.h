#pragma once

#include <vector>
#include <iostream>
#include "image.h"

class Brureforce_Search
{
    private:
        std::vector<Image*> images;

    public:
        Brureforce_Search(std::vector<Image*> images);
        // Find exact N Nearest Neighbours to query point q
        std::vector<std::pair<double, int>> exactNN(Image *q, int N);
        ~Brureforce_Search();
};
