#include "../headers/lsh.h"

LSH::LSH(int k,int w,int L,Dataset *imageDataset) {
    this->k = k;
    this->w = w;
    this->L = L;
    this->images = imageDataset->getImages();

    // Create hash tables
    this->hashTables = new Hash_Table*[L];
    for (int i = 0; i < L; i++) {
        this->hashTables[i] = new Hash_Table(k, w, imageDataset->getImageDimension(),this->images.size()/16);
    }

    // Insert all images there
    for (unsigned int i = 0; i < this->images.size(); i++) {
        for (int j = 0;j < L;j++) {
            this->hashTables[j]->insert(this->images[i]);
        }
    }
}

LSH::~LSH() {
    // Destroy all hash tables
    for (int i = 0; i < this->L; i++) {
        delete this->hashTables[i];
    }
    delete[] this->hashTables;
}