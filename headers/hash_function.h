#pragma once

// Implements functionality of hash function family H

#include "image.h"

class Hash_Function
{
    private:
        int k, w, M, d;
        unsigned long **s;
    public:
        Hash_Function(int k, int w, int d);
        // Calculate's hi
        unsigned long hash(Image *image, int k);
        ~Hash_Function();
};
