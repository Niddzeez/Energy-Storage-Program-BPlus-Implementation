#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HASH_TABLE_SIZE 1031

// Define maximum number of keys in B+ Tree (Adjust as needed)
#define MAX_KEYS 5
#define MIN_KEYS (MAX_KEYS / 2)

typedef enum {
    FAIL,
    SUCCESS
} status_code;

// Forward declaration to minimize cyclic dependencies
struct TransactionTreeNode;
struct BuyerList;
struct SellerBuyerPair;
struct Transaction;
struct Seller;
struct Buyer;
struct TransactionTreeNode;
struct SellerBTree;
struct BuyerBTreeNode;

// Transaction Structure
typedef struct Transaction {
    int transactionID;
    int buyerID;
    int sellerID;
    double energyAmount;   // kWh
    double pricePerKWh;
    double totalPrice;
    char timestamp[20];    // YYYY-MM-DD HH:MM:SS
} Transaction;

// Buyer Node (Stored in BST)
typedef struct Buyer {
    int buyerID;
    double totalEnergyPurchased;
    struct TransactionTreeNode *transactionTree; // Pointer to B+ Tree for buyer transactions
} Buyer;

//BuyerList
//implementing a linked list for 
typedef struct BuyerList{
    Buyer* buyer;
    struct BuyerList *next;
}BuyerList;

// Seller Structure
typedef struct Seller {
    int sellerID;                  // Unique identifier (used as key in B+ tree)
    double rateBelow300;           // Energy rate per kWh for usage below 300 kWh
    double rateAbove300;           // Energy rate per kWh for usage above 300 kWh
    double totalRevenue;           // Total revenue earned by this seller
    BuyerList *regularBuyers;  // List of regular buyers (Buyers with >5 transactions)
    struct TransactionTreeNode *transactionTree; // Pointer to a B+ tree for transactions
} Seller;

typedef struct SellerBuyerPair {
    Seller* seller;
    Buyer* buyer;
    int numTransactions;
    struct SellerBuyerPair* next; // For collision chaining
} SellerBuyerPair;

typedef struct TransactionTreeNode {
    int isLeaf;                                         // 1 if leaf, 0 if internal node
    int numKeys;                                        // Number of keys in node
    int keys[MAX_KEYS];                                 // Transaction IDs (for sorting)
    struct TransactionTreeNode *children[MAX_KEYS + 1]; // Child pointers (internal nodes)
    Transaction *transactions[MAX_KEYS]; // Only used in leaf nodes
    struct TransactionTreeNode *next;  // Pointer to next leaf node (for range queries)
} TransactionTreeNode;

// B+ Tree Node for sellers
typedef struct SellerBTree {
    int numKeys;                   // Number of keys present
    int keys[MAX_KEYS];            // Seller IDs as keys
    struct SellerBTree *children[MAX_KEYS + 1];  // Internal node pointers
    struct SellerBTree *next;      // Next pointer for linked leaf nodes
    Seller *sellers[MAX_KEYS];     // Pointers to seller records (only in leaf nodes)
    int isLeaf;                    // 1 if leaf node, 0 otherwise
} SellerBTree;

typedef struct BuyerBTreeNode {
    int isLeaf;                      // 1 if node is leaf, 0 otherwise
    int numKeys;                     // Number of keys currently in node
    int keys[MAX_KEYS];             // Sorted keys (buyerID)
    struct Buyer* buyers[MAX_KEYS]; // Pointers to Buyer structs
    struct BuyerBTreeNode* children[MAX_KEYS + 1]; // Child pointers
    struct BuyerBTreeNode* next;     // Next pointer for leaf node (for fast traversal)
} BuyerBTree;

#endif