#pragma once

#include <vector>
#include <iostream>
#include "image.h"

class Bruteforce_Search
{
    private:
        std::vector<Image*> images;

    public:
        Bruteforce_Search(std::vector<Image*> images);
        // Find exact N Nearest Neighbours to query point q
        std::vector<std::pair<double, int>> exactNN(Image *q, int N);
        ~Bruteforce_Search();
};
