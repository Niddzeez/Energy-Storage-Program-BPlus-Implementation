#ifndef BUYER_H
#define BUYER_H
#include "Structures.h"
#include "transaction.h"

BuyerBTree* createBuyerBTree(int isLeaf);
status_code insertBuyer(BuyerBTree **root, Buyer *newBuyer);
BuyerBTree* findLeafNodeBuyer(BuyerBTree *root, int buyerID);
BuyerBTree* findParentBuyer(BuyerBTree *root, BuyerBTree *leftNode);
BuyerBTree* splitAndInsertBuyer(BuyerBTree *root, BuyerBTree *leaf, Buyer* newBuyer);
void insertIntoLeafBuyer(BuyerBTree *leaf, Buyer* newBuyer);
Buyer* searchBuyer(BuyerBTree *root, int buyerID);
void displaybuyers(BuyerBTree *root);
BuyerBTree *insertIntoParentBuyer(BuyerBTree *root, BuyerBTree *leftNode, BuyerBTree *rightNode, int first_key);
BuyerBTree* splitAndPromoteParentBuyer(BuyerBTree *root, BuyerBTree *parent, BuyerBTree *rightNode, int key);
void insertIntoNonFullParentBuyer(BuyerBTree *parent, BuyerBTree *rightNode, int key);
Buyer* addBuyerFromTransaction(BuyerBTree **buyerTreeRoot, Transaction *trans);
Buyer* CreateBuyer(int BuyerID);
void PrintBuyer(Buyer *newBuyer);
void displayBuyerTransactionTrees(BuyerBTree *root);
void sortBuyerBasedOnEnergy(BuyerBTree* buyeroot);
void mergeBuyer(Buyer** temp, int left, int mid, int right);
void mergeSortBuyer(Buyer** temp, int left, int right);
void searchTransactionsByBuyerID(BuyerBTree* buyerRoot, int buyerID);
int compareBuyers(const void *a, const void *b);

#endif