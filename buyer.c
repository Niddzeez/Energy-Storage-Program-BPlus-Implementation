#include "buyer.h"

BuyerBTree* createBuyerBTree(int Leaf){
    BuyerBTree *newNode = (BuyerBTree*)malloc(sizeof(BuyerBTree));
    if(newNode){
        newNode->isLeaf = Leaf;
        newNode->numKeys = 0;
        for(int i = 0; i < MAX_KEYS+1; i++){
            newNode->children[i] = NULL;
        }
        for(int i = 0; i < MAX_KEYS; i++){
            newNode->keys[i] = -1;
        }
        newNode->next = NULL;
    }else{
        perror("Memory Allocation Failed!");
        exit(EXIT_FAILURE);
    }

    return newNode;
}

//helper function - find suitable leaf node for insertion of buyer
BuyerBTree* findLeafNodeBuyer(BuyerBTree *root, int buyerID) {
    if (root == NULL) {
        printf("The tree is empty.\n");
        return NULL;
    }
    BuyerBTree *current = root;

    while (!current->isLeaf) {
        int i = 0;
        while (i < current->numKeys && buyerID > current->keys[i]) {
            i++;
        }
        current = current->children[i];
    }

    printf("Leaf node found for buyerID: %d\n", buyerID);

    return current;
}

//helper function - inserts directly into leaf if leaf is not full
void insertIntoLeafBuyer(BuyerBTree *leaf, Buyer* newBuyer){
    int i;

    for(i = leaf->numKeys-1; i >= 0 && leaf->keys[i] > newBuyer->buyerID; i--){
        leaf->keys[i+1] = leaf->keys[i];
        leaf->buyers[i+1] = leaf->buyers[i];
    }

    leaf->keys[i+1] = newBuyer->buyerID;
    leaf->buyers[i+1] = newBuyer; 
    leaf->numKeys++;

    printf("Inserted into leaf\n");
}

//helper function - find the parent node of a given node
BuyerBTree* findParentBuyer(BuyerBTree *root, BuyerBTree *leftNode){
    printf("Finding parents\n");
    if (root == NULL || root->isLeaf) {
        return NULL;
    }
    for (int i = 0; i <= root->numKeys; i++) {
        if (root->children[i] == NULL) continue;
        if (root->children[i] == leftNode) {
            return root;
        }
        BuyerBTree *parent = findParentBuyer(root->children[i], leftNode);
        if (parent) return parent;
    }
    return NULL;
}

// Helper function - splits and promotes a full parent
BuyerBTree* splitAndPromoteParentBuyer(BuyerBTree *root, BuyerBTree *parent, BuyerBTree *rightNode, int key) {
    int totalKeys = parent->numKeys + 1;  // parent's keys plus the new key.
    
    // Allocate temporary arrays for keys and children (one extra slot).
    int *tempKeys = (int *)malloc(sizeof(int) * (MAX_KEYS + 1));
    BuyerBTree **tempChildren = (BuyerBTree **)malloc(sizeof(BuyerBTree*) * (MAX_KEYS + 2));
    
    // Copy the existing keys and children from the parent.
    tempChildren[0] = parent->children[0];
    for (int i = 0; i < parent->numKeys; i++) {
        tempKeys[i] = parent->keys[i];
        tempChildren[i + 1] = parent->children[i + 1];
    }
    
    // Find the insertion index for the new key.
    int insertIndex = 0;
    while (insertIndex < parent->numKeys && tempKeys[insertIndex] < key) {
        insertIndex++;
    }
    // Shift keys and children in the temporary arrays.
    for (int i = parent->numKeys; i > insertIndex; i--) {
        tempKeys[i] = tempKeys[i - 1];
        tempChildren[i + 1] = tempChildren[i];
    }
    tempKeys[insertIndex] = key;
    tempChildren[insertIndex + 1] = rightNode;
    
    // Determine the split index.
    int splitIndex = totalKeys / 2;
    int upKey = tempKeys[splitIndex];  // Key to be promoted.
    
    // Update the existing parent with the first half of the keys.
    parent->numKeys = splitIndex;
    for (int i = 0; i < splitIndex; i++) {
        parent->keys[i] = tempKeys[i];
    }
    for (int i = 0; i < splitIndex + 1; i++) {
        parent->children[i] = tempChildren[i];
    }
    
    // Create a new internal node for the second half.
    BuyerBTree *newInternal = createBuyerBTree(0);
    newInternal->numKeys = totalKeys - splitIndex - 1;
    for (int i = 0; i < newInternal->numKeys; i++) {
        newInternal->keys[i] = tempKeys[splitIndex + 1 + i];
    }
    for (int i = 0; i < newInternal->numKeys + 1; i++) {
        newInternal->children[i] = tempChildren[splitIndex + 1 + i];
    }
    
    free(tempKeys);
    tempKeys = NULL;
    free(tempChildren);
    tempChildren = NULL;
    
    // Recursively insert the promoted key into the parent's parent.
    return insertIntoParentBuyer(root, parent, newInternal, upKey);
}

// Helper function - Inserts key and right child into a non-full parent.
void insertIntoNonFullParentBuyer(BuyerBTree *parent, BuyerBTree *rightNode, int key) {
    int insertIndex = 0;
    // Find the correct position for the new key.
    while (insertIndex < parent->numKeys && parent->keys[insertIndex] < key) {
        insertIndex++;
    }
    // Shift keys and child pointers to make room.
    for (int i = parent->numKeys; i > insertIndex; i--) {
        parent->keys[i] = parent->keys[i - 1];
        parent->children[i + 1] = parent->children[i];
    }
    parent->keys[insertIndex] = key;
    parent->children[insertIndex + 1] = rightNode;
    parent->numKeys++;
}

//Helper Function - inserts into parent
BuyerBTree *insertIntoParentBuyer(BuyerBTree *root, BuyerBTree *leftNode, BuyerBTree *rightNode, int first_key) {
    printf("Insert into parent\n");
    // Base case: if leftNode is the root, then create a new root.
    if (leftNode == root) {
        printf("The root is split\n");
        BuyerBTree *newRoot = createBuyerBTree(0);
        newRoot->keys[0] = first_key;
        newRoot->numKeys = 1;
        newRoot->children[0] = leftNode;
        newRoot->children[1] = rightNode;
        return newRoot;  // Return the new root immediately.
    } else {
        BuyerBTree *parent = findParentBuyer(root, leftNode);
        // Case 1: Parent is not full.
        if (parent->numKeys < MAX_KEYS) {
            printf("The parent is not full\n");
            insertIntoNonFullParentBuyer(parent, rightNode, first_key);
            return root;
        }
        // Case 2: Parent is full.
        else {
            printf("The parent is full\n");
            return splitAndPromoteParentBuyer(root, parent, rightNode, first_key);
        }
    }
}

//helper function - splits the leaf node and then inserts the Seller
BuyerBTree* splitAndInsertBuyer(BuyerBTree *root, BuyerBTree *leaf, Buyer* newBuyer){

    printf("Entered split and insert\n");
    BuyerBTree *leftNode = leaf;
    BuyerBTree *rightNode = createBuyerBTree(1);
    
    //make a temp array to store all keys and buyers
    int *tempKeys = (int *)malloc(sizeof(int) * (MAX_KEYS+1));
    Buyer **tempBuyers = (Buyer**)malloc(sizeof(Buyer*) * (MAX_KEYS + 1));
    
    // Copy existing keys and buyers from the leaf into temp arrays
    for (int i = 0; i < leaf->numKeys; i++) {
        tempKeys[i] = leaf->keys[i];
        tempBuyers[i] = leaf->buyers[i];
    }
    
    //insert a new Buyer
    int i = MAX_KEYS - 1;
    while (i >= 0 && tempKeys[i] > newBuyer->buyerID) {
        tempKeys[i + 1] = tempKeys[i];
        tempBuyers[i + 1] = tempBuyers[i];
        i--;
    }
    tempKeys[i + 1] = newBuyer->buyerID;
    tempBuyers[i + 1] = newBuyer;
    
    //split and insert in leftNode and rightNode
    int splitIndex = (MAX_KEYS + 1) / 2;
    leftNode->numKeys = splitIndex;
    rightNode->numKeys = MAX_KEYS + 1 - splitIndex;
    
    for (i = 0; i < leftNode->numKeys; i++) {
        leftNode->keys[i] = tempKeys[i];
        leftNode->buyers[i] = tempBuyers[i];
    }
    
    for (i = 0; i < rightNode->numKeys; i++) {
        rightNode->keys[i] = tempKeys[splitIndex + i];
        rightNode->buyers[i] = tempBuyers[splitIndex + i];
    }

    rightNode->next = leftNode->next;
    leftNode->next = rightNode;

    free(tempKeys);
    tempKeys = NULL;
    free(tempBuyers);
    tempBuyers = NULL;

    return insertIntoParentBuyer(root, leaf, rightNode, rightNode->keys[0]);
}

//main function to insert the Seller
status_code insertBuyer(BuyerBTree **root, Buyer *newBuyer){
    status_code sc = SUCCESS;
    printf("Inserting Buyer\n");
    if(!(*root)){
        (*root) = createBuyerBTree(1);
        (*root)->keys[0] = newBuyer->buyerID;
        (*root)->buyers[0] = newBuyer;
        (*root)->numKeys++;
        printf("is a root node\n");
    }                                                      
    else
    {
        printf("is not a root node\n");
        BuyerBTree *leaf = findLeafNodeBuyer((*root), newBuyer->buyerID);

        if(leaf->numKeys < MAX_KEYS){
            //directly insert the Seller
            insertIntoLeafBuyer(leaf, newBuyer);
        }
        else{
            printf("split and insert");
            //leaf is full so split the node
            (*root) = splitAndInsertBuyer((*root), leaf, newBuyer);
        }
    }
    printf("Inserted Buyer\n");
    return sc;
}

Buyer* addBuyerFromTransaction(BuyerBTree **buyerTreeRoot, Transaction *trans) {
    // Look up the seller in the seller B+ tree
    Buyer *buyer = searchBuyer(*buyerTreeRoot, trans->buyerID);
    
    if (buyer == NULL) {
        // Seller not found, so create one.
        // (You can decide on appropriate energy rates; here we use example values.)
        buyer = CreateBuyer(trans->buyerID);
        // Insert the new seller into the seller B+ tree.
        insertBuyer(buyerTreeRoot, buyer);
    }
    
    // Now, insert the transaction into the seller's transaction subtree.
    if (buyer->transactionTree == NULL) {
        // If empty, create a new transaction tree (leaf node).
        buyer->transactionTree = createBPTreeNode(1);
    }
    insertTransaction(&buyer->transactionTree, trans);
    
    // update the seller's total revenue.
    buyer->totalEnergyPurchased += trans->energyAmount;

    return buyer;
}

//main function 2
Buyer* searchBuyer(BuyerBTree *root, int buyerID){
    Buyer *newTrans = NULL;
    BuyerBTree *find = findLeafNodeBuyer(root, buyerID);
    if(find){
        printf("The node exists\n");
        //we found the node that might contain the Seller
        //we search the node to see whether the Seller exists
        int i;
        int found = 0;
        for(i = 0; i < find->numKeys && !found; i++){
            if(find->keys[i] == buyerID){
                found = 1;
                newTrans = find->buyers[i];
            }
        }
    }
    else printf("The node does not exist");
    return newTrans;
}

//main function 3
void displaybuyers(BuyerBTree *root){
    BuyerBTree *leaf = root;
    while(!leaf->isLeaf){
        leaf = leaf->children[0];
    }

    //we get leaf node
    while(leaf){
        for(int i = 0; i < leaf->numKeys; i++){
            printf("\n\n");
            PrintBuyer(leaf->buyers[i]);
            printf("\n\n");
        }
        leaf = leaf->next;
    }
}

Buyer* CreateBuyer(int BuyerID) {
    Buyer *newBuyer = (Buyer *)malloc(sizeof(Buyer));
    if (!newBuyer) {
        perror("Failed to allocate Buyer");
        exit(EXIT_FAILURE);
    }
    newBuyer->buyerID = BuyerID;
    newBuyer->totalEnergyPurchased = 0.0;  // Initialize total energy purchased to 0
    newBuyer->transactionTree = NULL;      // Start with an empty transaction subtree
    return newBuyer;
}

// Function to print buyer details
void PrintBuyer(Buyer *newBuyer) {
    if (newBuyer == NULL) {
        printf("Buyer is NULL.\n");
        return;
    }

    printf("==============================================\n");
    printf("| Buyer ID              : %-15d     |\n", newBuyer->buyerID);
    printf("| Total Energy Purchased: %-15.2lf     |\n", newBuyer->totalEnergyPurchased);
    printf("| Transaction Tree       : %-15s     |\n", 
           (newBuyer->transactionTree == NULL) ? "Empty" : "Available");
    printf("==============================================\n");
}

// Function to display transaction trees for each buyer
void displayBuyerTransactionTrees(BuyerBTree *root) {
    if (root == NULL) {
        printf("Buyer tree is empty.\n");
        return;
    }

    BuyerBTree *leaf = root;
    // Traverse to the leftmost leaf node
    while (!leaf->isLeaf) {
        leaf = leaf->children[0];
    }

    // Traverse all leaf nodes
    while (leaf) {
        for (int i = 0; i < leaf->numKeys; i++) {
            Buyer *buyer = leaf->buyers[i];
            printf("\n==============================================\n");
            printf("Transactions for Buyer ID: %d\n", buyer->buyerID);
            printf("==============================================\n");
            if (buyer->transactionTree) {
                displayTransactions(buyer->transactionTree);
            } else {
                printf("No transactions available for this buyer.\n");
            }
        }
        leaf = leaf->next;
    }
}

//helper function - MergeSort
// Helper function to merge two halves
void mergeBuyer(Buyer** temp, int left, int mid, int right) {
    int n1 = mid - left + 1; // Size of the left subarray
    int n2 = right - mid;    // Size of the right subarray

    // Create temporary arrays
    Buyer** leftArray = (Buyer**)malloc(n1 * sizeof(Buyer*));
    Buyer** rightArray = (Buyer**)malloc(n2 * sizeof(Buyer*));

    // Copy data to temporary arrays
    for (int i = 0; i < n1; i++) {
        leftArray[i] = temp[left + i];
    }
    for (int i = 0; i < n2; i++) {
        rightArray[i] = temp[mid + 1 + i];
    }

    // Merge the temporary arrays back into temp
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (leftArray[i]->totalEnergyPurchased <= rightArray[j]->totalEnergyPurchased) {
            temp[k] = leftArray[i];
            i++;
        } else {
            temp[k] = rightArray[j];
            j++;
        }
        k++;
    }

    // Copy remaining elements of leftArray, if any
    while (i < n1) {
        temp[k] = leftArray[i];
        i++;
        k++;
    }

    // Copy remaining elements of rightArray, if any
    while (j < n2) {
        temp[k] = rightArray[j];
        j++;
        k++;
    }

    // Free temporary arrays
    free(leftArray);
    leftArray = NULL;
    free(rightArray);
    rightArray = NULL;
}

// Merge Sort function
void mergeSortBuyer(Buyer** temp, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // Sort the first and second halves
        mergeSortBuyer(temp, left, mid);
        mergeSortBuyer(temp, mid + 1, right);

        // Merge the sorted halves
        mergeBuyer(temp, left, mid, right);
    }
}

int compareBuyers(const void *a, const void *b) {
    Buyer *buyerA = *(Buyer **)a;
    Buyer *buyerB = *(Buyer **)b;
    
    if (buyerA->totalEnergyPurchased < buyerB->totalEnergyPurchased)
        return -1;
    else if (buyerA->totalEnergyPurchased > buyerB->totalEnergyPurchased)
        return 1;
    else
        return 0;
}


//main function - sorting buyers based on energy brought
void sortBuyerBasedOnEnergy(BuyerBTree* buyerRoot) {
    if (!buyerRoot) return;

    // Determine how many buyers we have by traversing the leaf nodes.
    // Here, we assume TransactionCount (or a similar global) represents a rough upper bound.
    int maxCount = TransactionCount;  
    Buyer** temp = (Buyer**)malloc(sizeof(Buyer*) * maxCount);
    if (!temp) {
        printf("Memory allocation failed for temp array.\n");
        return;
    }

    BuyerBTree* leaf = buyerRoot;
    while (!leaf->isLeaf) {
        leaf = leaf->children[0];
    }

    int j = 0;
    while (leaf) {
        for (int i = 0; i < leaf->numKeys; i++) {
            temp[j++] = leaf->buyers[i];
        }
        leaf = leaf->next;
    }

    // Sort the array using qsort
    qsort(temp, j, sizeof(Buyer*), compareBuyers);

    // Display sorted buyers
    printf("Displaying all Buyers in sorted order according to energy bought:\n");
    for (int i = 0; i < j; i++) {
        PrintBuyer(temp[i]);
        printf("%d\n", i);
    }

    free(temp);
    temp = NULL;
}


void searchTransactionsByBuyerID(BuyerBTree* buyerRoot, int buyerID) {
    Buyer* buyer = searchBuyer(buyerRoot, buyerID);
    if (!buyer) {
        printf("Buyer ID %d not found.\n", buyerID);
        return;
    }
    printf("Transactions for Buyer ID %d:\n", buyerID);
    displayTransactions(buyer->transactionTree);
}

