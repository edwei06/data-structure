#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_LENGTH 4

typedef struct BranchNode {
    struct BranchNode* left;
    struct BranchNode* right;
    struct BranchNode* parent;
    struct ElementNode* element;
    int bitNumber;
} BranchNode;

typedef struct ElementNode {
    char key[KEY_LENGTH]; 
} ElementNode;

BranchNode* createBranchNode() {
    BranchNode* node = (BranchNode*)malloc(sizeof(BranchNode));
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->element = NULL;
    node->bitNumber = 0;
    return node;
}

ElementNode* createElementNode(char* key) {
    ElementNode* node = (ElementNode*)malloc(sizeof(ElementNode));
    strncpy(node->key, key, KEY_LENGTH);
    return node;
}

int dontReturn = 0;

void *insert(BranchNode *root, char *key, int depth) {
    if (root == NULL) {
        BranchNode* node = createBranchNode();
        node->element = createElementNode(key);
        node->bitNumber = depth;
        return node;
    }

    if (root->element != NULL) {
        // 找到第一個不同的位元
        int diffBit = 0;
        while (diffBit < KEY_LENGTH && root->element->key[diffBit] == key[diffBit]) {
            diffBit++;
        }

        if(diffBit > root->bitNumber){ //在bitNum後才不同=>往下插入
            BranchNode* oldElement = createBranchNode();
            BranchNode* newElement = createBranchNode();
            oldElement->element = createElementNode(root->element->key);
            oldElement->bitNumber = diffBit;
            newElement->element = createElementNode(key);
            newElement->bitNumber = diffBit;
            if(root->element->key[diffBit] == '0'){
                root->left = oldElement;
                root->right = newElement;
                oldElement->parent = root;
                newElement->parent = root;
            }else{
                root->right = oldElement;
                root->left = newElement;
                oldElement->parent = root;
                newElement->parent = root;
            }
            root->bitNumber = diffBit;
            root->element = NULL;
        }else{//在bitNum前就不同=>往上插入
            BranchNode* newBranch = createBranchNode();
            newBranch->bitNumber = diffBit;

            BranchNode* rootParent = root->parent;
            BranchNode* temp = rootParent;
            if (rootParent->bitNumber > diffBit)
            {
                temp = rootParent;
                rootParent = rootParent->parent;
            }
            rootParent = temp;
            if(rootParent->parent->left == rootParent) rootParent->parent->left = newBranch;
            else rootParent->parent->right = newBranch;
            newBranch->parent = rootParent->parent;
            
            BranchNode* newElement = createBranchNode();
            newElement->element = createElementNode(key);
            newElement->bitNumber = diffBit;

            if(root->element->key[diffBit] == '0'){
                newBranch->left = rootParent;
                newBranch->right = newElement;
            }else{
                newBranch->right = rootParent;
                newBranch->left = newElement;
            }
            rootParent->parent = newBranch;
            newElement->parent = newBranch;

            dontReturn = 1;
        }
        return root;
    }

    // 根據當前比較位元決定往左或右
    if (key[root->bitNumber] == '0') {
        BranchNode* store = insert(root->left, key, root->bitNumber);
        if(dontReturn == 0){
            root->left = store;
            root->left->parent = root;
        }
    } else {
        BranchNode* store = insert(root->right, key, root->bitNumber);
        if(dontReturn == 0){
            root->right = store;
            root->right->parent = root;
        }
    }
    return root;
}
void insertKey(BranchNode *root, char *key, int depth){
    dontReturn = 0;
    insert(root, key, depth);
    return;
}//如果有前綴不同需要回頭插入branch 會在最後一次insert一次處理完 繼續return賦值會重置 所以用dontReturn當判斷 每次insert都需要重置 所以額外用一個insertKey包起來

BranchNode* search(BranchNode* root, char* key) {
    if (root == NULL) {
        return NULL;
    }

    if (root->element != NULL) {
        if (strcmp(root->element->key, key) == 0) return root;
        else return NULL;
    }

    if (key[root->bitNumber] == '0') {
        return search(root->left, key);
    } else {
        return search(root->right, key);
    }
}

void reduce(BranchNode* node) {
    if (node == NULL) return;
    
    if(node->left == NULL){//剩右邊
        if(node->right->element != NULL){//是element
            node->element = node->right->element;
            node->bitNumber = node->parent->bitNumber;
            node->right = NULL;
        }else{//是branch
            node->bitNumber = node->right->bitNumber;
            node->left = node->right->left;
            node->right = node->right->right;
            node->left->parent = node;
            node->right->parent = node;
        }
    }else if(node->right == NULL){//剩左邊
        if(node->left->element != NULL){//是element
            node->element = node->left->element;
            node->bitNumber = node->parent->bitNumber;
            node->left = NULL;
        }else{//是branch
            node->bitNumber = node->left->bitNumber;
            node->right = node->left->right;
            node->left = node->left->left;
            node->left->parent = node;
            node->right->parent = node;
        }
    }
}

void delete(BranchNode* root, char* key) {
    BranchNode* target = search(root, key);
    if (target == NULL) return;
    
    BranchNode* parent = target->parent;

    if (parent->left == target) {
        parent->left = NULL;
    } else {
        parent->right = NULL;
    }
    reduce(parent);
}

void levelOrder(BranchNode* root) {
    if (root == NULL) {
        return;
    }

    BranchNode* queue[100];  // 假設最多有 100 個節點
    int front = 0, rear = 0;

    queue[rear++] = root;

    while (front < rear) {
        BranchNode* node = queue[front++];

        if (node->element != NULL) {
            printf("%s:", node->element->key);
        }else{printf("B:");} //print Branch node 如果不需要刪掉else就好
        printf("%d  ",node->bitNumber);

        if (node->left != NULL) {
            queue[rear++] = node->left;
        }

        if (node->right != NULL) {
            queue[rear++] = node->right;
        }
    }

    printf("\n");
}

int main() {
    BranchNode* root = createBranchNode();

    insertKey(root, "0000", 0);
    insertKey(root, "0010", 0);
    insertKey(root, "1000", 0);
    insertKey(root, "1001", 0);
    insertKey(root, "1100", 0);
    insertKey(root, "1101", 0);
    insertKey(root, "0011", 0);
    insertKey(root, "0100", 0);

    BranchNode* result = search(root, "0011");
    if (result) {
        printf("Found key: %s\n", result->element->key);
    } else {
        printf("Key not found\n");
    }
    result = search(root, "0001");
    if (result) {
        printf("Found key: %s\n", result->element->key);
    } else {
        printf("Key not found\n");
    }

    levelOrder(root);

    delete(root, "0010");
    delete(root, "1001");
    printf("Key deleted\n");
    
    levelOrder(root);

    return 0;
}
