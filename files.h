#ifndef FILES_H
#define FILES_H
#include "seller.h"
#include "buyer.h"

#define TRANSACTION_FILE "transactions.csv"
#define SELLER_FILE "sellers.csv"
#define BUYER_FILE "buyers.csv"

void saveTransactionsToCSV(TransactionTreeNode* root);
void saveBuyersToCSV(BuyerBTree* root);
void saveSellersToCSV(SellerBTree* root);
void loadTransactionsFromCSV(TransactionTreeNode** transRoot, SellerBTree** selleroot, BuyerBTree** buyeroot);
void saveAll(TransactionTreeNode** transRoot, SellerBTree** selleroot, BuyerBTree** buyeroot);
void loadAll(TransactionTreeNode** transRoot, SellerBTree** selleroot, BuyerBTree** buyeroot);

#endif