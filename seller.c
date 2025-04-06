#include "seller.h"

SellerBTree* createSellerBTree(int Leaf){
    SellerBTree *newNode = (SellerBTree*)malloc(sizeof(SellerBTree));
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

//hepler function - find suitable leaf node for insertion of seller
SellerBTree* findLeafNodeSeller(SellerBTree *root, int sellerID){
    if (root == NULL) {
        printf("The tree is empty.\n");
        return NULL;
    }

    SellerBTree *current = root;
    printf("Starting to find the leaf node for sellerID: %d\n", sellerID);

    while (!current->isLeaf) {
        int i = 0;
        // Find the correct child to traverse
        while (i < current->numKeys && sellerID > current->keys[i]) {
            i++;
        }
        current = current->children[i];
    }

    printf("Leaf node found for sellerID: %d\n", sellerID);
    return current;
}

//helper function - inserts directly into leaf if leaf is not full
void insertIntoLeafSeller(SellerBTree *leaf, Seller* newSeller){
    int i;

    for(i = leaf->numKeys-1; i >= 0 && leaf->keys[i] > newSeller->sellerID; i--){
        leaf->keys[i+1] = leaf->keys[i];
        leaf->sellers[i+1] = leaf->sellers[i];
    }

    leaf->keys[i+1] = newSeller->sellerID;
    leaf->sellers[i+1] = newSeller; 
    leaf->numKeys++;

    printf("Inserted into leaf\n");
}

//helper function - find the parent node of a given node
SellerBTree* findParentSeller(SellerBTree *root, SellerBTree *leftNode){
    printf("Finding parents\n");
    if (root == NULL || root->isLeaf) {
        return NULL;
    }
    for (int i = 0; i <= root->numKeys; i++) {
        if (root->children[i] == NULL) continue;
        if (root->children[i] == leftNode) {
            return root;
        }
        SellerBTree *parent = findParentSeller(root->children[i], leftNode);
        if (parent) return parent;
    }
    return NULL;
}

//Helper function - splits and promotes a full parent
SellerBTree* splitAndPromoteParentSeller(SellerBTree *root, SellerBTree *parent, SellerBTree *rightNode, int key) {
    int totalKeys = parent->numKeys + 1;  // parent's keys plus the new key.
    
    // Allocate temporary arrays for keys and children (one extra slot).
    int *tempKeys = (int *)malloc(sizeof(int) * (MAX_KEYS + 1));
    SellerBTree **tempChildren = (SellerBTree **)malloc(sizeof(SellerBTree*) * (MAX_KEYS + 2));
    
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
    SellerBTree *newInternal = createSellerBTree(0);
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
    return insertIntoParentSeller(root, parent, newInternal, upKey);
}

// Helper function - Inserts key and right child into a non-full parent.
void insertIntoNonFullParentSeller(SellerBTree *parent, SellerBTree *rightNode, int key) {
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
SellerBTree *insertIntoParentSeller(SellerBTree *root, SellerBTree *leftNode, SellerBTree *rightNode, int first_key) {
    printf("Insert into parent\n");
    // Base case: if leftNode is the root, then create a new root.
    if (leftNode == root) {
        printf("The root is split\n");
        SellerBTree *newRoot = createSellerBTree(0);
        newRoot->keys[0] = first_key;
        newRoot->numKeys = 1;
        newRoot->children[0] = leftNode;
        newRoot->children[1] = rightNode;
        return newRoot;  // Return the new root immediately.
    } else {
        SellerBTree *parent = findParentSeller(root, leftNode);
        // Case 1: Parent is not full.
        if (parent->numKeys < MAX_KEYS) {
            printf("The parent is not full\n");
            insertIntoNonFullParentSeller(parent, rightNode, first_key);
            return root;
        }
        // Case 2: Parent is full.
        else {
            printf("The parent is full\n");
            return splitAndPromoteParentSeller(root, parent, rightNode, first_key);
        }
    }
}

//helper function - splits the leaf node and then inserts the Seller
SellerBTree* splitAndInsertSeller(SellerBTree *root, SellerBTree *leaf, Seller* newSeller){

    printf("Entered split and insert\n");
    SellerBTree *leftNode = leaf;
    SellerBTree *rightNode = createSellerBTree(1);

    //make a temp array to store all keys and sellers
    int *tempKeys = (int *)malloc(sizeof(int) * (MAX_KEYS+1));
    Seller **tempsellers = (Seller**)malloc(sizeof(Seller*) * (MAX_KEYS + 1));

    // Copy existing keys and sellers from the leaf into temp arrays
    for (int i = 0; i < leaf->numKeys; i++) {
        tempKeys[i] = leaf->keys[i];
        tempsellers[i] = leaf->sellers[i];
    }

    //insert a new Seller
    int i = MAX_KEYS - 1;
    while (i >= 0 && tempKeys[i] > newSeller->sellerID) {
        tempKeys[i + 1] = tempKeys[i];
        tempsellers[i + 1] = tempsellers[i];
        i--;
    }
    tempKeys[i + 1] = newSeller->sellerID;
    tempsellers[i + 1] = newSeller;

    //split and insert in leftNode and rightNode
    int splitIndex = (MAX_KEYS + 1) / 2;
    leftNode->numKeys = splitIndex;
    rightNode->numKeys = MAX_KEYS + 1 - splitIndex;

    for (i = 0; i < leftNode->numKeys; i++) {
        leftNode->keys[i] = tempKeys[i];
        leftNode->sellers[i] = tempsellers[i];
    }

    for (i = 0; i < rightNode->numKeys; i++) {
        rightNode->keys[i] = tempKeys[splitIndex + i];
        rightNode->sellers[i] = tempsellers[splitIndex + i];
    }

    rightNode->next = leftNode->next;
    leftNode->next = rightNode;

    free(tempKeys);
    tempKeys = NULL;
    free(tempsellers);
    tempsellers = NULL;

    return insertIntoParentSeller(root, leaf, rightNode, rightNode->keys[0]);
}

//main function to insert the Seller
status_code insertSeller(SellerBTree **root, Seller *newSeller){
    status_code sc = SUCCESS;
    printf("Inserting Seller\n");
    if(!(*root)){
        (*root) = createSellerBTree(1);
        (*root)->keys[0] = newSeller->sellerID;
        (*root)->sellers[0] = newSeller;
        (*root)->numKeys++;
        printf("is a root node\n");
    }                                                      
    else
    {
        printf("is not a root node\n");
        SellerBTree *leaf = findLeafNodeSeller((*root), newSeller->sellerID);

        if(leaf->numKeys < MAX_KEYS){
            //directly insert the Seller
            insertIntoLeafSeller(leaf, newSeller);
        }
        else{
            printf("split and insert");
            //leaf is full so split the node
            (*root) = splitAndInsertSeller((*root), leaf, newSeller);
        }
    }
    printf("Inserted Seller\n");
    return sc;
}

Seller* addSellerFromTransaction(SellerBTree **sellerTreeRoot, Transaction *trans) {
    // Look up the seller in the seller B+ tree
    printf("Entered into add seller from transactions\n");
    Seller *seller = searchSeller(*sellerTreeRoot, trans->sellerID);
    
    if (seller == NULL) {
        printf("Seller is NULL\n");
        // Seller not found, so create one.
        // (You can decide on appropriate energy rates; here we use example values.)
        seller = CreateSeller(trans->sellerID);
        printf("Created a seller\n");
        // Insert the new seller into the seller B+ tree.
        insertSeller(sellerTreeRoot, seller);
        printf("Inserted a seller\n");
    }
    
    // Now, insert the transaction into the seller's transaction subtree.
    if (seller->transactionTree == NULL) {
        // If empty, create a new transaction tree (leaf node).
        seller->transactionTree = createBPTreeNode(1);
    }
    insertTransaction(&seller->transactionTree, trans);
    printf("Inserted transaction in seller subtree\n");
    
    // update the seller's total revenue.
    seller->totalRevenue += trans->totalPrice;
    printf("Insertion completed seller added");

    return seller;
}

//main function 2
Seller* searchSeller(SellerBTree *root, int sellerID){
    printf("Enterd search seller");
    Seller *newTrans = NULL;
    SellerBTree *find = findLeafNodeSeller(root, sellerID);

    if(find){
        printf("The node exists\n");
        //we found the node that might contain the Seller
        //we search the node to see whether the Seller exists
        int i;
        int found = 0;
        for(i = 0; i < find->numKeys && !found; i++){
            if(find->keys[i] == sellerID){
                found = 1;
                newTrans = find->sellers[i];
            }
        }
    }
    else printf("The node does not exist");
    return newTrans;
}

//main function 3
void displaysellers(SellerBTree *root){
    SellerBTree *leaf = root;
    while(!leaf->isLeaf){
        leaf = leaf->children[0];
    }

    //we get leaf node
    while(leaf){
        for(int i = 0; i < leaf->numKeys; i++){
            printf("\n\n");
            PrintSeller(leaf->sellers[i]);
            printf("\n\n");
        }
        leaf = leaf->next;
    }
}

Seller* CreateSeller(int SellerID) {
    Seller *newSeller = (Seller *)malloc(sizeof(Seller));
    if (!newSeller) {
        perror("Failed to allocate Seller");
        exit(EXIT_FAILURE);
    }
    newSeller->sellerID = SellerID;
    newSeller->rateBelow300 = 0.0;
    newSeller->rateAbove300 = 0.0;
    newSeller->totalRevenue = 0.0;
    newSeller->regularBuyers = NULL;      // Initialize the regular buyers list as empty
    newSeller->transactionTree = NULL;    // Start with an empty transaction subtree
    return newSeller;
}

//function to print seller
void PrintSeller(Seller *newseller) {
    if (newseller == NULL) {
        printf("Seller is NULL.\n");
        return;
    }

    printf("==============================================\n");
    printf("| Seller ID            : %-15d     |\n", newseller->sellerID);
    printf("| Rate Below 300 kWh   : %-15.2lf     |\n", newseller->rateBelow300);
    printf("| Rate Above 300 kWh   : %-15.2lf     |\n", newseller->rateAbove300);
    printf("| Total Revenue        : %-15.2lf     |\n", newseller->totalRevenue);
    printf("| Regular Buyers       : %-15s     |\n", 
           (newseller->regularBuyers == NULL) ? "None" : "Available");
    printf("| Transaction Tree     : %-15s     |\n", 
           (newseller->transactionTree == NULL) ? "Empty" : "Available");
    printf("==============================================\n");
}

// Function to display transaction trees for each seller
void displaySellerTransactionTrees(SellerBTree *root) {
    if (root == NULL) {
        printf("Seller tree is empty.\n");
        return;
    }

    SellerBTree *leaf = root;
    // Traverse to the leftmost leaf node
    while (!leaf->isLeaf) {
        leaf = leaf->children[0];
    }

    // Traverse all leaf nodes
    while (leaf) {
        for (int i = 0; i < leaf->numKeys; i++) {
            Seller *seller = leaf->sellers[i];
            printf("\n==============================================\n");
            printf("Transactions for Seller ID: %d\n", seller->sellerID);
            printf("==============================================\n");
            if (seller->transactionTree) {
                displayTransactions(seller->transactionTree);
            } else {
                printf("No transactions available for this seller.\n");
            }
        }
        leaf = leaf->next;
    }
}

int calculateTotalRevenue(SellerBTree *root, int sellerID){
    if(!root) return 0;
    
    Seller* seller = searchSeller(root, sellerID);
    if(seller){
        return seller->totalRevenue;
    }

    printf("The seller doesnt exist!\n");
    return 0;
}

void searchTransactionsBySellerID(SellerBTree* sellerRoot, int sellerID) {
    Seller* seller = searchSeller(sellerRoot, sellerID);
    if (!seller) {
        printf("Seller ID %d not found.\n", sellerID);
        return;
    }
    printf("Transactions for Seller ID %d:\n", sellerID);
    displayTransactions(seller->transactionTree);
}