#include "../headers/hypercube.h"
#include "../headers/utilities.h"
#include <cmath>
#include <iostream>
//#include <bitset>

Hypercube::Hypercube(Dataset *imageDataset, int dimension, int w) {
    // Initialize arguments
    std::vector<Image*> images = imageDataset->getImages();
    this->dimension = dimension;
    this->w = w;
    this->d = imageDataset->getImageDimension();

    // Initialize hash function
    this->h = new Hash_Function(this->dimension, this->w, this->d);

    // Initialize hash maps for fi functions
    this->f = new std::unordered_map<unsigned long, int>[this->dimension];

    // Initialize containers for projection points
    this->vertices = new std::list<Image*>[power(2, this->dimension)];

    // Insert all the images from the dataset to the hypercube
    for(unsigned int i = 0; i < images.size(); i++) {
        this->insert(images[i]);
    }
}


unsigned int Hypercube::hash(Image *image) {
    unsigned int result = 0;
    // Generate hash value (project vector to a vertice in the hypercube)
    for (int i = 0; i < this->dimension; i++) {
        unsigned long hash = this->h->hash(image,this->dimension - 1 - i);
        // Check if we have already generated a bit for the d - i th hash
        if (this->f[this->dimension - 1 - i].find(hash) == this->f[this->dimension - 1 - i].end()) {
            // If not, generate it now by flipping a coin
            this->f[this->dimension - 1 - i][hash] = rand() % 2;
        }
        
        result |= (this->f[this->dimension - 1 - i][hash] << i);
    }
    
    return result;
}


bool Hypercube::insert(Image *image) {
    this->vertices[this->hash(image)].push_back(image);
    return true;
}


std::list<Image*> Hypercube::search(unsigned int M, int probes, int curVertex, int checked_mask) {
    // Base case
    if (probes <= 0) {
        std::list<Image*> emptyList;
        return emptyList;
    }
    //std::bitset<32> x(curVertex),y(checked_mask);
    //std::cout << "Vertex: " << x << " Hammed bits: " << y << std::endl;
    // Search current vertex
    std::list<Image*> result = this->vertices[curVertex];
    if (result.size() > M) {
        result.resize(M);
    }
    M -= result.size();
    probes--;

    // Recursively search non searched neighbour vertices (hamming)
    for (int i = 0; M > 0 && probes > 0 && i < this->dimension; i++) {
        // Check if ith bit was not previously hammed 
        if (!(checked_mask & (1 << i))) {
            std::list<Image*> rec = search(M, probes - this->dimension, curVertex ^ (1 << i), checked_mask | (1 << i));
            M -= rec.size();
            probes--;
            result.splice(result.end(), rec);
        }
    }
    
    return result;
}


std::list<std::pair<double,int>> Hypercube::searchSimilarPoints(Image *q,int M, int probes) {
    std::list<Image*> ret = this->search(M,probes,this->hash(q),0);
    // std::cout << ret.size() << std::endl;
    std::list<std::pair<double,int>> result;
    
    for (std::list<Image*>::iterator it = ret.begin(); it != ret.end(); it++) {
        std::pair<double,int> tmp((*it)->distance(q,1),(*it)->getId());
        result.push_back(tmp);
    }
    
    //result.sort();
    //ret.resize(M);
    return result;
}

Hypercube::~Hypercube() {
    delete[] this->vertices;
    delete[] this->f;
    delete this->h;
}