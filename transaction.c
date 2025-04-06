#include "transaction.h"
long int TransactionCount = 0;

//creating a new node for the tree
TransactionTreeNode* createBPTreeNode(int Leaf){
    TransactionTreeNode *newNode = (TransactionTreeNode*)malloc(sizeof(TransactionTreeNode));
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

//hepler function - find suitable leaf node for insertion of transaction
TransactionTreeNode* findLeafNode(TransactionTreeNode *root, int transactionID){
    TransactionTreeNode *current = root;
    
    while (!current->isLeaf) {
        int i = 0;
        while (i < current->numKeys && transactionID > current->keys[i]) {
            i++;
        }
        current = current->children[i];
    }

    printf("Finding leaf node for %d\n", transactionID);

    return current;
}

//helper function - inserts directly into leaf if leaf is not full
void insertIntoLeaf(TransactionTreeNode *leaf, Transaction* newTransaction){
    int i;

    for(i = leaf->numKeys-1; i >= 0 && leaf->keys[i] > newTransaction->transactionID; i--){
        leaf->keys[i+1] = leaf->keys[i];
        leaf->transactions[i+1] = leaf->transactions[i];
    }

    leaf->keys[i+1] = newTransaction->transactionID;
    leaf->transactions[i+1] = newTransaction; 
    leaf->numKeys++;

    printf("Inserted into leaf\n");
}

//helper function - find the parent node of a given node
TransactionTreeNode* findParent(TransactionTreeNode *root, TransactionTreeNode *leftNode){
    printf("Finding parents\n");
    // Base case: if the root is a leaf or its children are leaves, then it cannot be the parent of 'child'
    if (root == NULL || root->isLeaf) {
        return NULL;
    }
    
    // Check if any of the children of the current node is the child we are looking for
    for (int i = 0; i <= root->numKeys; i++) {
        if (root->children[i] == leftNode) {
            return root;
        }
    }
    
    // Recursively traverse the tree to find the parent.
    for (int i = 0; i <= root->numKeys; i++) {
        TransactionTreeNode *parent = findParent(root->children[i], leftNode);
        if (parent != NULL) {
            return parent;
        }
    }
    
    // If the parent is not found, return NULL.
    return NULL;
}

//Helper function - splits and promotes a full parent
TransactionTreeNode* splitAndPromoteParent(TransactionTreeNode *root, TransactionTreeNode *parent, TransactionTreeNode *rightNode, int key) {
    int totalKeys = parent->numKeys + 1;  // parent's keys plus the new key.
    
    // Allocate temporary arrays for keys and children (one extra slot).
    int *tempKeys = (int *)malloc(sizeof(int) * (MAX_KEYS + 1));
    TransactionTreeNode **tempChildren = (TransactionTreeNode **)malloc(sizeof(TransactionTreeNode*) * (MAX_KEYS + 2));
    
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
    TransactionTreeNode *newInternal = createBPTreeNode(0);
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
    return insertIntoParent(root, parent, newInternal, upKey);
}

// Helper function - Inserts key and right child into a non-full parent.
void insertIntoNonFullParent(TransactionTreeNode *parent, TransactionTreeNode *rightNode, int key) {
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
TransactionTreeNode *insertIntoParent(TransactionTreeNode *root, TransactionTreeNode *leftNode, TransactionTreeNode *rightNode, int first_key) {
    printf("Insert into parent\n");
    // Base case: if leftNode is the root, then create a new root.
    if (leftNode == root) {
        printf("The root is split\n");
        TransactionTreeNode *newRoot = createBPTreeNode(0);
        newRoot->keys[0] = first_key;
        newRoot->numKeys = 1;
        newRoot->children[0] = leftNode;
        newRoot->children[1] = rightNode;
        return newRoot;  // Return the new root immediately.
    } else {
        TransactionTreeNode *parent = findParent(root, leftNode);
        // Case 1: Parent is not full.
        if (parent->numKeys < MAX_KEYS) {
            printf("The parent is not full\n");
            insertIntoNonFullParent(parent, rightNode, first_key);
            return root;
        }
        // Case 2: Parent is full.
        else {
            printf("The parent is full\n");
            return splitAndPromoteParent(root, parent, rightNode, first_key);
        }
    }
}

//helper function - splits the leaf node and then inserts the transaction
TransactionTreeNode* splitAndInsert(TransactionTreeNode *root, TransactionTreeNode *leaf, Transaction* newTransaction){

    printf("Entered split and insert\n");
    TransactionTreeNode *leftNode = leaf;
    TransactionTreeNode *rightNode = createBPTreeNode(1);

    //make a temp array to store all keys and transactions
    int *tempKeys = (int *)malloc(sizeof(int) * (MAX_KEYS+1));
    Transaction **tempTransactions = (Transaction**)malloc(sizeof(Transaction*) * (MAX_KEYS + 1));

    // Copy existing keys and transactions from the leaf into temp arrays
    for (int i = 0; i < leaf->numKeys; i++) {
        tempKeys[i] = leaf->keys[i];
        tempTransactions[i] = leaf->transactions[i];
    }

    //insert a new transaction
    int i = MAX_KEYS - 1;
    while (i >= 0 && tempKeys[i] > newTransaction->transactionID) {
        tempKeys[i + 1] = tempKeys[i];
        tempTransactions[i + 1] = tempTransactions[i];
        i--;
    }
    tempKeys[i + 1] = newTransaction->transactionID;
    tempTransactions[i + 1] = newTransaction;

    //split and insert in leftNode and rightNode
    int splitIndex = (MAX_KEYS + 1) / 2;
    leftNode->numKeys = splitIndex;
    rightNode->numKeys = MAX_KEYS + 1 - splitIndex;

    for (i = 0; i < leftNode->numKeys; i++) {
        leftNode->keys[i] = tempKeys[i];
        leftNode->transactions[i] = tempTransactions[i];
    }

    for (i = 0; i < rightNode->numKeys; i++) {
        rightNode->keys[i] = tempKeys[splitIndex + i];
        rightNode->transactions[i] = tempTransactions[splitIndex + i];
    }

    rightNode->next = leftNode->next;
    leftNode->next = rightNode;

    free(tempKeys);
    tempKeys = NULL;
    free(tempTransactions);
    tempTransactions = NULL;

    return insertIntoParent(root, leaf, rightNode, rightNode->keys[0]);
}

//main function to insert the transaction
void insertTransaction(TransactionTreeNode **root, Transaction *newTransaction){
    status_code sc = SUCCESS;
    printf("Inserting transaction\n");
    if(!(*root)){
        (*root) = createBPTreeNode(1);
        (*root)->keys[0] = newTransaction->transactionID;
        (*root)->transactions[0] = newTransaction;
        (*root)->numKeys++;
        printf("is a root node\n");
    }                                                           
    else{
        printf("is not a root node\n");
        TransactionTreeNode *leaf = findLeafNode((*root), newTransaction->transactionID);

        if(leaf->numKeys < MAX_KEYS){
            //directly insert the transaction
            insertIntoLeaf(leaf, newTransaction);
        }
        else{
            printf("split and insert");
            //leaf is full so split the node
            (*root) = splitAndInsert((*root), leaf, newTransaction);
            if(root) printf("done\n");
        }
    }

    printf("Inserted transaction\n");
}

//main function 2
Transaction* searchTransaction(TransactionTreeNode *root, int transactionID){
    Transaction *newTrans = NULL;
    TransactionTreeNode *find = findLeafNode(root, transactionID);
    if(find){
        printf("The node exists\n");
        //we found the node that might contain the transaction
        //we search the node to see whether the transaction exists
        int i;
        int found = 0;
        for(i = 0; i < find->numKeys && !found; i++){
            if(find->keys[i] == transactionID){
                found = 1;
                newTrans = find->transactions[i];
            }
        }
    }
    else printf("The node does not exist");
    return newTrans;
}

//main function 3
void displayTransactions(TransactionTreeNode *root){
    TransactionTreeNode *leaf = root;
    printf("Entered display transactions\n");
    while(!leaf->isLeaf){
        printf("Moved to the leaf");
        leaf = leaf->children[0];
    }
    printf("Checked root\n");
    //we get leaf node
    while(leaf){
        printf("into while loop\n");
        for(int i = 0; i < leaf->numKeys; i++){
            printf("\n\n");
            PrintTransaction(leaf->transactions[i]);
            printf("\n\n");
        }
        leaf = leaf->next;
    }
    printf("Exited");
}

Transaction* CreateTransaction(int buyerID, int sellerID, double energyAmount, double pricePerKWh) {
    Transaction* newTrans = (Transaction*)malloc(sizeof(Transaction));
    if (!newTrans) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    newTrans->transactionID = ++TransactionCount;
    newTrans->buyerID = buyerID;
    newTrans->sellerID = sellerID;
    newTrans->energyAmount = energyAmount;
    newTrans->pricePerKWh = pricePerKWh;
    newTrans->totalPrice = energyAmount * pricePerKWh;

    // Generate a timestamp in the format "YYYY-MM-DD HH:MM:SS"
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(newTrans->timestamp, sizeof(newTrans->timestamp), "%Y-%m-%d %H:%M:%S", t);

    return newTrans;
}

void PrintTransaction(Transaction *newTrans) {
    if (newTrans == NULL) {
        printf("Transaction is NULL.\n");
        return;
    }

    printf("==============================================\n");
    printf("| Transaction ID       : %-15d     |\n", newTrans->transactionID);
    printf("| Buyer ID             : %-15d     |\n", newTrans->buyerID);
    printf("| Seller ID            : %-15d     |\n", newTrans->sellerID);
    printf("| Energy Amount (kWh)  : %-15.2lf     |\n", newTrans->energyAmount);
    printf("| Price per kWh        : %-15.2lf     |\n", newTrans->pricePerKWh);
    printf("| Total Price          : %-15.2lf     |\n", newTrans->totalPrice);
    printf("| Timestamp            : %-15s |\n", newTrans->timestamp);
    printf("==============================================\n");
}
/*
//helper function - MergeSort
// Helper function to merge two halves
void merge(Transaction** temp, int left, int mid, int right) {
    printf("Entered merge operation\n");
    int n1 = mid - left + 1; // Size of the left subarray
    int n2 = right - mid;    // Size of the right subarray
    printf("Merging from %d to %d (n1 = %d, n2 = %d)\n", left, right, n1, n2);
    // Create temporary arrays
    Transaction** leftArray = (Transaction**)malloc(n1 * sizeof(Transaction*));
    Transaction** rightArray = (Transaction**)malloc(n2 * sizeof(Transaction*));

    if (!leftArray || !rightArray) {
        printf("Memory allocation failed in merge()\n");
        exit(EXIT_FAILURE); // Or handle gracefully
    }

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
        if (leftArray[i]->energyAmount <= rightArray[j]->energyAmount) {
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
    printf("Merged\n");
}

// Merge Sort function
void mergeSort(Transaction** temp, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // Sort the first and second halves
        mergeSort(temp, left, mid);
        printf("Merged first part\n");
        mergeSort(temp, mid + 1, right);
        printf("merged the second part\n");

        // Merge the sorted halves
        merge(temp, left, mid, right);
        printf("Finally merged\n");
    }
}

// Merge function using pre-allocated auxiliary array
void merge(Transaction** temp, int left, int mid, int right, Transaction** aux) {
    printf("Entered merge operation\n");
    int i = left, j = mid + 1, k = left;

    printf("Merging from %d to %d (mid = %d)\n", left, right, mid);

    while (i <= mid && j <= right) {
        if (temp[i]->energyAmount <= temp[j]->energyAmount) {
            aux[k++] = temp[i++];
        } else {
            aux[k++] = temp[j++];
        }
    }

    while (i <= mid) aux[k++] = temp[i++];
    while (j <= right) aux[k++] = temp[j++];

    // Copy back to original array
    for (int i = left; i <= right; i++) {
        temp[i] = aux[i];
    }

    printf("Merged\n");
}

// Updated merge sort with auxiliary buffer
void mergeSort(Transaction** temp, int left, int right, Transaction** aux) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(temp, left, mid, aux);
        printf("Merged first part\n");
        mergeSort(temp, mid + 1, right, aux);
        printf("Merged second part\n");
        merge(temp, left, mid, right, aux);
        printf("Finally merged\n");
    }
}

*/
//helper function - scan the BTree and fill the array up
int scanEnergyAmountBetween(TransactionTreeNode* transroot, double min, double max, Transaction** temp){
    TransactionTreeNode *leaf = transroot;
    if (!transroot) {
        printf("Transaction tree is empty.\n");
        return 0;
    }
    while (!leaf->isLeaf) {
        leaf = leaf->children[0];
    }
    int j = 0;

    while(leaf){
        printf("Scanning leaf node with %d transactions...\n", leaf->numKeys);
        for(int i = 0; i < leaf->numKeys; i++ ){
            printf("Checking energyAmount: %.2lf\n", leaf->transactions[i]->energyAmount);
            if(leaf->transactions[i]->energyAmount >= min && leaf->transactions[i]->energyAmount <= max){
                temp[j++] = leaf->transactions[i];
                printf("Transaction ID: %d | Address: %p\n", leaf->transactions[i]->transactionID, (void*)leaf->transactions[i]);
            }
            printf("Added transaction with energyAmount %.2lf to temp[%d]\n", leaf->transactions[i]->energyAmount, j-1);
        }
        leaf = leaf->next;
    }

    return j;
}
/*
void displayEnergyAmountBetween(TransactionTreeNode** transroot, double min, double max){
    int count = TransactionCount;

    Transaction** temp = (Transaction**)malloc(sizeof(Transaction*) * count); // array for storing filtered transactions
    Transaction** aux = (Transaction**)malloc(sizeof(Transaction*) * count); // auxiliary array for merge sort

    if (!temp || !aux) {
        printf("Memory allocation for temp or aux failed.\n");
        free(temp);
        free(aux);
        return;
    }
    
    // scan the tree and fill temp with relevant transactions
    printf("Going to scan transactions...\n");
    int j = scanEnergyAmountBetween(*transroot, min, max, temp);
    printf("Number of transactions in range: %d\n", j);

    printf("Scanned array\n");
    mergeSort(temp, 0, j-1, aux); // pass aux array here
    printf("Array merged\n");

    printf("Displaying all transactions with energy amount between %.2lf and %.2lf:\n", min, max);
    for (int i = 0; i < j; i++) {
        PrintTransaction(temp[i]);
        printf("\n");
    }

    // cleanup
    free(temp);
    free(aux);
    temp = NULL;
    aux = NULL;

    printf("Displayed all transactions!\n");
}*/

int compareTransactions(const void *a, const void *b) {
    Transaction *t1 = *(Transaction**)a;
    Transaction *t2 = *(Transaction**)b;
    if (t1->energyAmount < t2->energyAmount) return -1;
    else if (t1->energyAmount > t2->energyAmount) return 1;
    else return 0;
}

void displayEnergyAmountBetween(TransactionTreeNode** transroot, double min, double max) {
    int count = TransactionCount; // Ensure TransactionCount is set correctly
    Transaction** temp = (Transaction**)malloc(sizeof(Transaction*) * count);
    if (!temp) {
        printf("Memory allocation for temp failed.\n");
        return;
    }

    // Scan the B+ tree and fill temp with transactions in the given energy range
    printf("Going to scan transactions...\n");
    int j = scanEnergyAmountBetween(*transroot, min, max, temp);
    printf("Number of transactions in range: %d\n", j);

    if (j == 0) {
        printf("No transactions found in the given range.\n");
        free(temp);
        return;
    }

    // Sort using qsort with our comparator
    qsort(temp, j, sizeof(Transaction*), compareTransactions);
    printf("Transactions sorted using qsort.\n");

    // Display the sorted transactions
    printf("Displaying all transactions with energy amount between %.2lf and %.2lf:\n", min, max);
    for (int i = 0; i < j; i++) {
        PrintTransaction(temp[i]);
        printf("%d\n", i+1);
    }

    free(temp);
    printf("Displayed all transactions!\n");
}


//main function 4
void findTransactionsByTime(TransactionTreeNode *root, char *startTime, char *endTime) {
    if(root == NULL) {
        printf("Tree is empty!\n");
    }
    else{
        printf("The tree is not empty\n");
        //Traverse from the root down to the leftmost leaf.
        TransactionTreeNode *leaf = root;
        while(!leaf->isLeaf) {
            leaf = leaf->children[0];
        }
        
        // In the leaf level, use binary search to find the first transaction 
        // with timestamp >= startTime. If not found in the current leaf, move to the next.
        int pos = -1;
        while(leaf && pos == -1) {
            pos = binarySearchLeaf(leaf, startTime);
            printf("binary search complete\n");
            if(pos == -1) {
                // No transaction in this leaf meets startTime, move to next leaf.
                leaf = leaf->next;
            }
        }

        
        // If no leaf contains a transaction with timestamp >= startTime, nothing to print.
        if(leaf == NULL) {
            printf("No transactions found in the given time range.\n");
        }
        else{
            printf("trying to find transaction.. \n");
            // Now traverse from this leaf forward, printing transactions as long as
            // the timestamp does not exceed endTime.
            while(leaf) {
                for(int i = pos; i < leaf->numKeys ; i++) {
                    // If transaction timestamp exceeds endTime, we are done.
                    if(strcmp(leaf->transactions[i]->timestamp, endTime) > 0) {
                        printf("Highest endtime found\n");
                        return;
                    }
                    // Print the transaction.

                    PrintTransaction(leaf->transactions[i]);
                    printf("\n");
                }
                // For subsequent leaves, start from the first element.
                pos = 0;

                leaf = leaf->next;
                printf("Jumped on the next leaf\n");
            }
        }
    }
}

//helper function - binary search
// Function to perform a binary search in a leaf node for the first transaction
// with a timestamp not less than startTime. Returns the index if found, else -1.
int binarySearchLeaf(TransactionTreeNode *leaf, char *startTime) {
    int low = 0, high = leaf->numKeys - 1, pos = -1;
    while(low <= high) {
        int mid = (low + high) / 2;
        int cmp = strcmp(leaf->transactions[mid]->timestamp, startTime);
        if(cmp >= 0) {  
            pos = mid;
            high = mid - 1;  
        } else {
            low = mid + 1;
        }
    }
    return pos;
}
