#pragma once

#include <unordered_map>
#include "image.h"

class Cluster
{
    private:
        Image *centroid;
        std::unordered_map<int,Image*> points;
    public:
        Cluster(Image &centroid);
        bool addPoint(Image* point);
        bool removePoint(int id);
        unsigned int getSize();
        void updateCentroid();
        Image* getCentroid();
        double avgDistance(Image *point);
        ~Cluster();
};

