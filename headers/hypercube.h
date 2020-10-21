#pragma once

#include <unordered_map>
#include <list>
#include "image.h"
#include "hash_function.h"
#include "../headers/dataset.h"

class Hypercube
{
    private:
        int dimension,w,d;// d is dimension of the vectors
        Hash_Function *h;
        std::unordered_map<unsigned long,int> *f;
        std::list<Image*> *vertices;
        unsigned int hash(Image *image);
        std::list<Image*> search(unsigned int M, int probes, int curVertex, int checked_mask);
    public:
        Hypercube(Dataset *dataset, int dimension, int w);
        bool insert(Image *image);
        // Returns M similar points sorted by their distance from q in ascending order
        std::list<std::pair<double, int>> searchSimilarPoints(Image *q, int M, int probes);
        ~Hypercube();
};