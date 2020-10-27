#include "../headers/cluster.h"

Cluster::Cluster(Image &centroid) {
    this->centroid = new Image(centroid);
}

bool Cluster::addPoint(Image *point) {
    // Check if this point is already in this cluster
    if (this->points.find(point->getId()) == this->points.end()) {
        this->points[point->getId()] = point;
        return true;
    } else {
        return false;
    }
}

unsigned int Cluster::getSize() {
    return this->points.size();
}

Image* Cluster::getCentroid() {
    return this->centroid;
}

Cluster::~Cluster() {
    delete this->centroid;
}