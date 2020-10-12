#pragma once

#include <vector>
#include "image.h"
#include "dataset.h"
#include "hash_table.h"

class LSH
{
    private:
        int k, w, L;
        std::vector<Image*> images;
        Hash_Table **hashTables;
    public:
        LSH(int k,int w,int L,Dataset *imageDataset);
        ~LSH();
};