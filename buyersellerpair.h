#ifndef BUYERSELLERPAIR_H
#define BUYERSELLERPAIR_H
#include "Structures.h"

extern SellerBuyerPair* hashTable[HASH_TABLE_SIZE];

int hash(Seller* seller, Buyer* buyer);
void updateSellerBuyerPair(Seller* seller, Buyer* buyer);
SellerBuyerPair** collectPairs(int* count);
void quickSort(SellerBuyerPair** arr, int low, int high);
void displayAllSellerBuyerPairsSorted();
void freeHashTable();
void printHashTable();

#endif