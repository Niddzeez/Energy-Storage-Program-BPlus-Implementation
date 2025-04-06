#include "buyersellerpair.h"

SellerBuyerPair* hashTable[HASH_TABLE_SIZE] = { NULL };

int hash(Seller* seller, Buyer* buyer) {
    return ((seller->sellerID * 31) ^ (buyer->buyerID * 17)) % HASH_TABLE_SIZE;
}

void updateSellerBuyerPair(Seller* seller, Buyer* buyer) {
    int idx = hash(seller, buyer);
    if (idx < 0) idx += HASH_TABLE_SIZE;
    SellerBuyerPair* head = hashTable[idx];

    // Check if the pair exists
    while (head != NULL) {
        if (head->seller->sellerID == seller->sellerID &&
            head->buyer->buyerID == buyer->buyerID) {

            seller = head->seller;
            buyer = head->buyer;

            head->numTransactions++;
            if (head->numTransactions == 6) {
                BuyerList* newNode = (BuyerList*)malloc(sizeof(BuyerList));
                newNode->buyer = buyer;
                newNode->next = seller->regularBuyers;
                seller->regularBuyers = newNode;
            }
            return;
        }
        head = head->next;
    }

    // If not found, add new
    SellerBuyerPair* newPair = (SellerBuyerPair*)malloc(sizeof(SellerBuyerPair));
    newPair->seller = seller;
    newPair->buyer = buyer;
    newPair->numTransactions = 1;
    newPair->next = hashTable[idx];
    hashTable[idx] = newPair;
}

SellerBuyerPair** collectPairs(int* count) {
    SellerBuyerPair** arr = (SellerBuyerPair**)malloc(sizeof(SellerBuyerPair*) * HASH_TABLE_SIZE); // oversize
    int idx = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        SellerBuyerPair* node = hashTable[i];
        while (node) {
            arr[idx++] = node;
            node = node->next;
        }
    }
    *count = idx;
    return arr;
}

void quickSort(SellerBuyerPair** arr, int low, int high) {
    if (low < high) {
        int pivot = arr[high]->numTransactions;
        int i = low - 1;

        for (int j = low; j < high; j++) {
            if (arr[j]->numTransactions >= pivot) {
                i++;
                SellerBuyerPair* temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }

        SellerBuyerPair* temp = arr[i+1];
        arr[i+1] = arr[high];
        arr[high] = temp;

        quickSort(arr, low, i);
        quickSort(arr, i + 2, high);
    }
}

void displayAllSellerBuyerPairsSorted() {
    int count = 0;
    SellerBuyerPair** pairs = collectPairs(&count);
    quickSort(pairs, 0, count - 1);

    printf("\n--- Seller-Buyer Pairs Sorted by Number of Transactions ---\n");
    for (int i = 0; i < count; i++) {
        printf("Seller ID: %d | Buyer ID: %d | Transactions: %d\n",
               pairs[i]->seller->sellerID,
               pairs[i]->buyer->buyerID,
               pairs[i]->numTransactions);
    }

    free(pairs);
    pairs = NULL;
}

void freeHashTable() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        SellerBuyerPair* curr = hashTable[i];
        while (curr) {
            SellerBuyerPair* next = curr->next;
            free(curr);
            curr = next;
        }
        hashTable[i] = NULL;
    }
}

void printHashTable() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        SellerBuyerPair* head = hashTable[i];
        while (head) {
            printf("Hash Index: %d | Seller ID: %d | Buyer ID: %d | Transactions: %d\n",
                   i, head->seller->sellerID, head->buyer->buyerID, head->numTransactions);
            head = head->next;
        }
    }
}
