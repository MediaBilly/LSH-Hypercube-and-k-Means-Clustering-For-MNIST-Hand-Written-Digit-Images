#include <iostream>
#include "../headers/hash_table.h"

Hash_Table::Hash_Table(int k, int w, int d, int buckets) {
    // Initialize arguments
    this->k = k;
    this->w = w;
    this->d = d;
    this->buckets = buckets;

    // Create buckets
    this->table = new std::list<Image*>[buckets]; 

    // Initialize hash function
    this->hash_function = new Hash_Function(k,w,d);
}


bool Hash_Table::insert(Image *image) {
    // Get value of g
    unsigned long g = 0;
    for (int i = 0; i < this->k; i++)
        g |= this->hash_function->hash(image, i) << (32 - i * k);
    
    // Insert the image on the appropriate bucket
    this->table[g % this->buckets].push_back(image);
    return true;
}

Hash_Table::~Hash_Table() {
    for(int i = 0; i < buckets; i++)
        this->table[i].clear();

    delete[] this->table;
    delete this->hash_function;
}
