#pragma once

#include <list>


class Hash_table
{ 
    int BUCKET;    // No. of buckets 
  
    // Pointer to an array containing buckets 
    list<int> *table; 
public: 
    Hash_table(int V);  // Constructor 
    ~Hash_table();
    // inserts a key into hash table 
    void insertItem(int x); 
  
    // deletes a key from hash table 
    void deleteItem(int key); 
  
    // hash function to map values to key 
    int hashFunction(int x) { 
        return (x % BUCKET); 
    } 
  
    void displayHash(); 
}; 