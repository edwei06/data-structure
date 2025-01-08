#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_LENGTH 4

typedef struct BranchNode {
    struct BranchNode* left;
    struct BranchNode* right;
    struct BranchNode* parent;
    struct ElementNode* element;
} BranchNode;

typedef struct ElementNode {
    char key[KEY_LENGTH]; 
} ElementNode;

BranchNode* createBranchNode() {
    BranchNode* node = (BranchNode*)malloc(sizeof(BranchNode));
    node->left = NULL;
    node->right = NULL;
    node->element = NULL;  
    return node;
}

ElementNode* createElementNode(char* key) {
    ElementNode* node = (ElementNode*)malloc(sizeof(ElementNode));
    strncpy(node->key, key, KEY_LENGTH);
    return node;
}

void *insert(BranchNode *root, char *key, int depth) {
    if (root == NULL) {
        BranchNode* node = createBranchNode();
        node->element = createElementNode(key);
        return node;
    }

    if (root->element != NULL) {
        if(root->element->key[depth] != key[depth]){
            BranchNode* oldElement = createBranchNode();
            BranchNode* newElement = createBranchNode();
            oldElement->element = createElementNode(root->element->key);
            newElement->element = createElementNode(key);
            if(root->element->key[depth] == '0'){
                root->left = oldElement;
                root->right = newElement;
            }else{
                root->right = oldElement;
                root->left = newElement;
            }
            root->element = NULL;
            root->left->parent = root;
            root->right->parent = root;
            return root;
        }else{
            BranchNode* newRoot = createBranchNode();
            newRoot->element = createElementNode(root->element->key);
            if(root->element->key[depth] == '0'){
                root->left = newRoot;
                root->left->parent = root;
                root->element = NULL;
                root->left = insert(root->left, key, depth + 1);
            }else{
                root->right = newRoot;
                root->right->parent = root;
                root->element = NULL;
                root->right = insert(root->right, key, depth + 1);
            }
            return root;
        }
    }

    // 根據鍵值的 bit 決定往左或往右
    if (key[depth] == '0') {
        root->left = insert(root->left, key, depth + 1);
        root->left->parent = root;
    } else {
        root->right = insert(root->right, key, depth + 1);
        root->right->parent = root;
    }

    return root;
}

BranchNode* search(BranchNode* root, char* key, int depth) {
    if (root == NULL) {
        return NULL;
    }

    if (root->element != NULL) {
        if(strcmp(root->element->key, key) == 0) return root;
        else return NULL;
    }

    // 根據鍵值的 bit 決定往左或往右
    if (key[depth] == '0') {
        return search(root->left, key, depth + 1);
    } else {
        return search(root->right, key, depth + 1);
    }

}

void reduce(BranchNode* previous, char* key) {
    if (previous->left == NULL || previous->right == NULL) {
        previous->left = NULL;
        previous->right = NULL;
        previous->element = createElementNode(key);
        reduce(previous->parent, key);
        return;
    }
    return;
}

// 刪除函數
void delete(BranchNode* root, char* key) {
    BranchNode* parent = search(root, key, 0)->parent;
    if (parent->left->element == NULL || parent->right->element == NULL)
    {
        if (parent->left->element && parent->left->element->key == key)parent->left = NULL;
        else parent->right = NULL;
        return;
    }

    char anotherNodKey[KEY_LENGTH];
    if (parent->left->element && strcmp(parent->left->element->key, key) == 0)
    {
        parent->left = NULL;
        strncpy(anotherNodKey, parent->right->element->key, KEY_LENGTH);
    }else{
        parent->right = NULL;
        strncpy(anotherNodKey, parent->left->element->key, KEY_LENGTH);
    }
    
    reduce(parent, anotherNodKey);
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
            printf("%s ", node->element->key);
        }else{printf("B ");} //print Branch node 如果不需要刪掉else就好

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

    insert(root, "0000", 0);
    insert(root, "0001", 0);
    insert(root, "0010", 0);
    insert(root, "0111", 0);
    insert(root, "1000", 0);
    insert(root, "1001", 0);
    insert(root, "1100", 0);
    insert(root, "1101", 0);

    levelOrder(root);

    BranchNode* result = search(root, "0011", 0);
    if (result) {
        printf("Found key: %s\n", result->element->key);
    } else {
        printf("Key not found\n");
    }

    delete(root, "0111");
    delete(root, "1100");
    printf("Key deleted\n");

    result = search(root, "0011", 0);
    if (result) {
        printf("Found key: %s\n", result->element->key);
    } else {
        printf("Key not found\n");
    }
    
    levelOrder(root);

    return 0;
}
