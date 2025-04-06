#ifndef SELLER_H
#define SELLER_H
#include "Structures.h"
#include "transaction.h"
#include "buyersellerpair.h"

// Seller B+ Tree functions
SellerBTree* createSellerBTree(int isLeaf);
status_code insertSeller(SellerBTree **root, Seller *newSeller);
SellerBTree* findLeafNodeSeller(SellerBTree *root, int sellerID);
SellerBTree* findParentSeller(SellerBTree *root, SellerBTree *leftNode);
SellerBTree* splitAndInsertSeller(SellerBTree *root, SellerBTree *leaf, Seller* newSeller);
void insertIntoLeafSeller(SellerBTree *leaf, Seller* newSeller);
Seller* searchSeller(SellerBTree *root, int sellerID);
void displaysellers(SellerBTree *root);
SellerBTree *insertIntoParentSeller(SellerBTree *root, SellerBTree *leftNode, SellerBTree *rightNode, int first_key);
SellerBTree* splitAndPromoteParentSeller(SellerBTree *root, SellerBTree *parent, SellerBTree *rightNode, int key);
void insertIntoNonFullParentSeller(SellerBTree *parent, SellerBTree *rightNode, int key);
Seller* addSellerFromTransaction(SellerBTree **sellerTreeRoot, Transaction *trans);
Seller* CreateSeller(int SellerID);
void PrintSeller(Seller *newseller);
void displaySellerTransactionTrees(SellerBTree *root);
int calculateTotalRevenue(SellerBTree *root, int sellerID);
void searchTransactionsBySellerID(SellerBTree* sellerRoot, int sellerID);


#endif