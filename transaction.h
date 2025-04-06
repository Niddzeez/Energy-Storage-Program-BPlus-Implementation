#ifndef TRANSACTION_H
#define TRANSACTION_H
#include "Structures.h"
#include<time.h>

extern long int TransactionCount;

// Transaction B+ Tree functions
TransactionTreeNode* createBPTreeNode(int isLeaf);
void insertTransaction(TransactionTreeNode **root, Transaction *newTransaction);
TransactionTreeNode* findLeafNode(TransactionTreeNode *root, int transactionID);
TransactionTreeNode* findParent(TransactionTreeNode *root, TransactionTreeNode *leftNode);
TransactionTreeNode* splitAndInsert(TransactionTreeNode *root, TransactionTreeNode *leaf, Transaction* newTransaction);
void insertIntoLeaf(TransactionTreeNode *leaf, Transaction* newTransaction);
Transaction* searchTransaction(TransactionTreeNode *root, int transactionID);
void displayTransactions(TransactionTreeNode *root);
TransactionTreeNode *insertIntoParent(TransactionTreeNode *root, TransactionTreeNode *leftNode, TransactionTreeNode *rightNode, int first_key);
TransactionTreeNode* splitAndPromoteParent(TransactionTreeNode *root, TransactionTreeNode *parent, TransactionTreeNode *rightNode, int key);
void insertIntoNonFullParent(TransactionTreeNode *parent, TransactionTreeNode *rightNode, int key);
Transaction* CreateTransaction(int buyerID, int sellerID, double energyAmount, double pricePerKWh);
void PrintTransaction(Transaction *newTrans);
void displayEnergyAmountBetween(TransactionTreeNode** transroot, double min, double max);
int scanEnergyAmountBetween(TransactionTreeNode* transroot, double min, double max, Transaction** temp);
int binarySearchLeaf(TransactionTreeNode *leaf, char *startTime);
void findTransactionsByTime(TransactionTreeNode *root, char *startTime, char *endTime);
//void mergeSort(Transaction** temp, int left, int right, Transaction** aux);
//void merge(Transaction** temp, int left, int mid, int right, Transaction** aux);
int compareTransactions(const void *a, const void *b);

#endif