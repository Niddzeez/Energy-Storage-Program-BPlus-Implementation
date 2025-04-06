#include "files.h"

void loadTransactionsFromCSV(TransactionTreeNode** transRoot, SellerBTree** selleroot, BuyerBTree** buyeroot) {
    FILE* file = fopen(TRANSACTION_FILE, "r");
    if (!file) {
        perror("No transactions file found to load");
        return;
    }

    char line[256];
    fgets(line, sizeof(line), file); // skip header

    int count = 0;
    while (fgets(line, sizeof(line), file)) {
        Transaction* t = (Transaction*)malloc(sizeof(Transaction));
        if (!t) {
            fprintf(stderr, "Memory allocation failed\n");
        }
        else{
            sscanf(line, "%d,%d,%d,%lf,%lf,%lf,%[^\n]",
                &t->transactionID, &t->buyerID, &t->sellerID,
                &t->energyAmount, &t->pricePerKWh, &t->totalPrice, t->timestamp);

            t->timestamp[strcspn(t->timestamp, "\r\n")] = 0;
            if (t->transactionID > TransactionCount) {
                TransactionCount = t->transactionID;
            }

            insertTransaction(transRoot, t);  

            Seller *seller = addSellerFromTransaction(selleroot, t);
            Buyer *buyer = addBuyerFromTransaction(buyeroot, t);
            updateSellerBuyerPair(seller, buyer);
            count++;
        }
    }

    printf("%d transactions loaded from %s\n", count, TRANSACTION_FILE);

    fclose(file);
}

void saveTransactionsToCSV(TransactionTreeNode* root) {
    FILE* file = fopen(TRANSACTION_FILE, "w");
    if (!file) {
        perror("Failed to open file for saving transactions");
        return;
    }

    // Write CSV header
    fprintf(file, "TransactionID,BuyerID,SellerID,EnergyAmount,PricePerKWh,TotalPrice,Timestamp\n");

    // Navigate to the leftmost leaf
    while (root && !root->isLeaf) {
        root = root->children[0];
    }

    int count = 0;

    // Traverse leaf level
    while (root) {
        for (int i = 0; i < root->numKeys; i++) {
            Transaction* t = root->transactions[i];
            fprintf(file, "%d,%d,%d,%.2lf,%.2lf,%.2lf,%s\n",
                    t->transactionID, t->buyerID, t->sellerID,
                    t->energyAmount, t->pricePerKWh, t->totalPrice, t->timestamp);
            count++;
        }
        root = root->next;
    }

    fclose(file);
    printf("%d transactions saved successfully to %s\n", count, TRANSACTION_FILE);
}

void saveSellersToCSV(SellerBTree* root) {
    FILE* file = fopen(SELLER_FILE, "w");
    if (!file) {
        perror("Failed to open file for saving sellers");
        return;
    }

    // Updated headers based on new struct
    fprintf(file, "SellerID,RateBelow300,RateAbove300,TotalRevenue\n");

    // Navigate to leftmost leaf
    while (root && !root->isLeaf) {
        root = root->children[0];
    }

    int count = 0;
    while (root) {
        for (int i = 0; i < root->numKeys; i++) {
            Seller* s = root->sellers[i];
            fprintf(file, "%d,%.2lf,%.2lf,%.2lf\n",
                    s->sellerID, s->rateBelow300, s->rateAbove300, s->totalRevenue);
            count++;
        }
        root = root->next;
    }

    fclose(file);
    printf("%d sellers saved to %s\n", count, SELLER_FILE);
}

void saveBuyersToCSV(BuyerBTree* root) {
    FILE* file = fopen(BUYER_FILE, "w");
    if (!file) {
        perror("Failed to open file for saving buyers");
        return;
    }

    fprintf(file, "BuyerID,TotalEnergyPurchased\n");

    // Navigate to leftmost leaf
    while (root && !root->isLeaf) {
        root = root->children[0];
    }

    int count = 0;
    while (root) {
        for (int i = 0; i < root->numKeys; i++) {
            Buyer* b = root->buyers[i];
            fprintf(file, "%d,%.2lf\n", b->buyerID, b->totalEnergyPurchased);
            count++;
        }
        root = root->next;
    }

    fclose(file);
    printf("%d buyers saved to %s\n", count, BUYER_FILE);
}

void saveAll(TransactionTreeNode** transRoot, SellerBTree** selleroot, BuyerBTree** buyeroot) {

    printf("\nSaving all data...\n");

    saveTransactionsToCSV(*transRoot);
    saveSellersToCSV(*selleroot);
    saveBuyersToCSV(*buyeroot);

    printf("All data saved successfully.\n");
}

void loadAll(TransactionTreeNode** transRoot, SellerBTree** selleroot, BuyerBTree** buyeroot) {
    printf("\nLoading all data...\n");

    loadTransactionsFromCSV(transRoot, selleroot, buyeroot);

    printf("All data loaded successfully.\n");
}

