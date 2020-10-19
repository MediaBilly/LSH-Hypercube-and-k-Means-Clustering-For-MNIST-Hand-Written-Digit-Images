#include <iostream>
#include <random>
#include "../headers/hash_function.h"
#include "../headers/utilities.h"

const unsigned long m = 4294967291; // m = (2^32) - 5


Hash_Function::Hash_Function(int k, int w, int d) {
    // Initialize parameters
    this->k = k;
    this->w = w;
    this->d = d;
    this->M = power(2, 32/k);
    
    // For each hash function generate it's Si's, 0 <= i <= d - 1
    this->s = new unsigned long*[k];
    for (int i = 0; i < this->k; i++) {
        this->s[i] = new unsigned long[d];
        for (int j = 0; j < this->d; j++) {
            this->s[i][j] = rand() % this->w;
        }
    }
}

unsigned long Hash_Function::hash(Image *image, int k) {
    if (k < 0 || k >= this->k) {
        return 0;
    }
    unsigned long cur_hash = 0, cur_m = 1;
    // Calculate current hi (ai's)
    for (int j = this->d - 1; j >= 0; j--) {
        cur_hash = (cur_hash + (cur_m * ((image->getPixel(j) - this->s[k][j])/this->w) % M) % M) % M;
        cur_m = ((m % this->M) * cur_m) % this->M;
    }
    return cur_hash;
}

Hash_Function::~Hash_Function() {
    for (int i = 0; i < this->k; i++)
        delete[] this->s[i];

    delete[] this->s;
}