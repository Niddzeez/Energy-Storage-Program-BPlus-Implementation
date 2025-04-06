#include "files.h"

int main() {
    TransactionTreeNode *transRoot = NULL;
    SellerBTree *sellerRoot = NULL;
    BuyerBTree *buyerRoot = NULL;

    loadAll(&transRoot, &sellerRoot, &buyerRoot);
    if (transRoot == NULL) {
        printf("Transroot is NULL after loading\n");
    } else {
        printf("Transroot is successfully loaded\n");
    }


    int choice;
    while (1) {
        printf("\n========== Energy Trading System ==========\n");
        printf("1. Add New Transaction\n");
        printf("2. Display All Transactions\n");
        printf("3. Create a Set of Transactions for Every Seller\n");
        printf("4. Create a Set of Transactions for Every Buyer\n");
        printf("5. Find Transactions in a Given Time Period\n");
        printf("6. Calculate Total Revenue by Seller\n");
        printf("7. Display Transactions with Energy in Range (Ascending Order)\n");
        printf("8. Sort Buyers Based on Energy Bought\n");
        printf("9. Sort Seller/Buyer Pairs by Number of Transactions\n");
        printf("10. Search particular Buyer or Seller\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 0) {
            saveAll(&transRoot, &sellerRoot, &buyerRoot);
            printf("Data saved. Exiting... Goodbye!\n");

            break;
        }

        switch (choice) {
            case 1: {
                int buyerID, sellerID;
                double energy, price;
                printf("Enter Buyer ID: ");
                scanf("%d", &buyerID);
                printf("Enter Seller ID: ");
                scanf("%d", &sellerID);
                printf("Enter Energy (kWh): ");
                scanf("%lf", &energy);
                printf("Enter Price per kWh: ");
                scanf("%lf", &price);

                Transaction *newTrans = CreateTransaction(buyerID, sellerID, energy, price);
                insertTransaction(&transRoot, newTrans);

                Seller *seller = addSellerFromTransaction(&sellerRoot, newTrans);
                Buyer *buyer = addBuyerFromTransaction(&buyerRoot, newTrans);
                updateSellerBuyerPair(seller, buyer);

                printf("Transaction successfully added.\n");
                break;
            }

            case 2:
                displayTransactions(transRoot);
                break;

            case 3:
                displaySellerTransactionTrees(sellerRoot);
                break;

            case 4:
                displayBuyerTransactionTrees(buyerRoot);
                break;

            case 5: {
                char start[20], end[20];
                printf("Enter Start Time (YYYY-MM-DD HH:MM): ");
                scanf(" %[^\n]", start);
                printf("Enter End Time (YYYY-MM-DD HH:MM): ");
                scanf(" %[^\n]", end);
                findTransactionsByTime(transRoot, start, end);
                break;
            }

            case 6: {
                int sellerID;
                printf("Enter Seller ID: ");
                scanf("%d", &sellerID);
                int revenue = calculateTotalRevenue(sellerRoot, sellerID);
                printf("Total Revenue for Seller %d: ₹%d\n", sellerID, revenue);
                break;
            }

            case 7: {
                double min, max;
                printf("Enter minimum energy amount: ");
                scanf("%lf", &min);
                printf("Enter maximum energy amount: ");
                scanf("%lf", &max);
                displayEnergyAmountBetween(&transRoot, min, max);
                break;
            }

            case 8:
                sortBuyerBasedOnEnergy(buyerRoot);
                break;

            case 9:
                displayAllSellerBuyerPairsSorted(); // uses your hashTable
                break;
            
            case 10: {
                int choice, id;
                printf("\n1. Search by Buyer ID\n2. Search by Seller ID\nEnter your choice: ");
                scanf("%d", &choice);
                if (choice == 1) {
                    printf("Enter Buyer ID: ");
                    scanf("%d", &id);
                    Buyer* buyer = searchBuyer(buyerRoot, id);
                    if(!buyer) printf("Buyer doesn't exist!\n");
                    PrintBuyer(buyer);
                    printf("\n");
                } else if (choice == 2) {
                    printf("Enter Seller ID: ");
                    scanf("%d", &id);
                    Seller* seller = searchSeller(sellerRoot, id);
                    if(!seller) printf("Seller doesn't exist!\n");
                    else PrintSeller(seller);
                    printf("\n");
                } else {
                    printf("Invalid Choice.\n");
                }
                break;
            }

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    freeHashTable(); 
    return 0;
}
