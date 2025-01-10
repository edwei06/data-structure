#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_KEYS 1000
typedef struct BTreeNode {
    int *keys;                  // Array of keys
    struct BTreeNode **children;// Pointers to child nodes
    int n;                      // The number of keys in the current node
    int isLeaf;                 // Whether the node is a leaf
} BTreeNode;

static int M;         
BTreeNode *root = NULL; 

BTreeNode* createNode(int isLeaf);
int compare_int(const void* a, const void* b);
BTreeNode* insertKey(BTreeNode *root, int key) ;
void collectLeafKeys(BTreeNode* node, int* arr, int* size, int maxSize);
void removeFromLeaf(BTreeNode *node, int idx);
int findKeyPosition(BTreeNode *node, int key);
void deleteKeyInternal(BTreeNode *node, int key);
void deleteKey(int key);


BTreeNode* createNode(int isLeaf) {
    BTreeNode *node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->keys = (int*)malloc((M - 1) * sizeof(int));
    node->children = (BTreeNode**)malloc(M * sizeof(BTreeNode*));
    node->n = 0;
    node->isLeaf = isLeaf;
    for(int i = 0; i < M; i++) {
        node->children[i] = NULL;
    }
    return node;
}

int compare_int(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void split_child(BTreeNode *parent, int index, BTreeNode *child) {
    BTreeNode *right = createNode(child->isLeaf);

    int mid = (MAX_KEYS + 1) / 2;
    int pivot = child->keys[mid - 1];

    right->n = child->n - mid;
    for (int i = 0; i < right->n; i++) {
        right->keys[i] = child->keys[mid + i];
    }

    if (!child->isLeaf) {
        for (int i = 0; i <= right->n; i++) {
            right->children[i] = child->children[mid + i];
        }
    }

    child->n = mid - 1;

    for (int i = parent->n; i > index; i--) {
        parent->keys[i] = parent->keys[i - 1];
        parent->children[i + 1] = parent->children[i];
    }
    parent->keys[index] = pivot;
    parent->children[index + 1] = right;
    parent->n++;
}
void insert_non_full(BTreeNode *node, int key) {
    int i = node->n - 1;
    if (node->n) {
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->n++;
    } else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->n == MAX_KEYS) {
            split_child(node, i, node->children[i]);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insert_non_full(node->children[i], key);
    }
}

BTreeNode* insertKey(BTreeNode *root, int key) {
    if (root->n == MAX_KEYS) {
        BTreeNode *new_root = createNode(0);
        new_root->children[0] = root;
        split_child(new_root, 0, root);

        int i = 0;
        if (key > new_root->keys[0]) {
            i++;
        }
        insert_non_full(new_root->children[i], key);
        return new_root;
    } else {
        insert_non_full(root, key);
        return root;
    }
}


void collectLeafKeys(BTreeNode* node, int* arr, int* size, int maxSize) {
    if (!node) return;
    if (node->isLeaf) {
        for (int i = 0; i < node->n; i++) {
            arr[*size] = node->keys[i];
            (*size)++;
        }
    } else {
        for (int i = 0; i <= node->n; i++) {
            collectLeafKeys(node->children[i], arr, size, maxSize);
        }
    }
}

void removeFromLeaf(BTreeNode *node, int idx) {

    for (int i = idx; i < node->n - 1; i++) {
        node->keys[i] = node->keys[i + 1];
    }
    node->n--;
}


int findKeyPosition(BTreeNode *node, int key) {
    int idx = 0;
    while (idx < node->n && node->keys[idx] < key) {
        idx++;
    }
    if (idx < node->n && node->keys[idx] == key) {
        return idx;
    }
    return idx;
}


void deleteKeyInternal(BTreeNode *node, int key) {
    int idx = 0;

    while (idx < node->n && key > node->keys[idx]) {
        idx++;
    }

    if (node->isLeaf) {

        int pos = findKeyPosition(node, key);
        if (pos != -1) {
            removeFromLeaf(node, pos);
        }
        return;
    } else {
        if (idx < node->n && node->keys[idx] == key) {
            return;
        }
        deleteKeyInternal(node->children[idx], key);

        BTreeNode *child = node->children[idx];
        int minKeys = (M - 1) / 2; 
        if (child->n < minKeys) {
            BTreeNode *leftSibling = (idx - 1 >= 0) ? node->children[idx-1] : NULL;
            BTreeNode *rightSibling = (idx + 1 <= node->n) ? node->children[idx+1] : NULL;
            if (rightSibling && rightSibling->n > minKeys) {
                child->keys[child->n] = node->keys[idx]; 
                child->n++;
                node->keys[idx] = rightSibling->keys[0];
                for(int i=0; i < rightSibling->n - 1; i++){
                    rightSibling->keys[i] = rightSibling->keys[i+1];
                }
                rightSibling->n--;
            }
            else if (leftSibling && leftSibling->n > minKeys) {
                for(int i=child->n; i>0; i--){
                    child->keys[i] = child->keys[i-1];
                }
                child->keys[0] = node->keys[idx-1];
                child->n++;
                node->keys[idx-1] = leftSibling->keys[leftSibling->n-1];
                leftSibling->n--;
            }
            else {
                if (rightSibling) {
                    child->keys[child->n] = node->keys[idx];
                    child->n++;
                    for(int i=0; i<rightSibling->n; i++){
                        child->keys[child->n+i] = rightSibling->keys[i];
                    }
                    child->n += rightSibling->n;

                    for(int i=idx; i<node->n-1; i++){
                        node->keys[i] = node->keys[i+1];
                        node->children[i+1] = node->children[i+2];
                    }
                    node->n--;

                    free(rightSibling->keys);
                    free(rightSibling->children);
                    free(rightSibling);
                }
                else if (leftSibling) {
                    leftSibling->keys[leftSibling->n] = node->keys[idx-1];
                    leftSibling->n++;
                    for(int i=0; i<child->n; i++){
                        leftSibling->keys[leftSibling->n + i] = child->keys[i];
                    }
                    leftSibling->n += child->n;

                    for(int i=idx-1; i<node->n-1; i++){
                        node->keys[i] = node->keys[i+1];
                        node->children[i] = node->children[i+1];
                    }
                    node->children[node->n] = NULL;
                    node->n--;

                    free(child->keys);
                    free(child->children);
                    free(child);
                    child = leftSibling;
                }
            }
        }
    }
}

void deleteKey(int key) {
    if (!root) return; 

    deleteKeyInternal(root, key);
    if (root->n == 0 && !root->isLeaf) {
        BTreeNode *oldRoot = root;
        root = root->children[0];
        free(oldRoot->keys);
        free(oldRoot->children);
        free(oldRoot);
    }
}
int main() {
    char command[32];
    int key;
    BTreeNode *root = createNode(1);
    scanf("m-way %d", &M);

    while (1) {
        if (scanf("%s", command) != 1) break;

        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "insert") == 0) {
            scanf("%d", &key);
            root = insertKey(root, key);
        } else if (strcmp(command, "delete") == 0) {
            scanf("%d", &key);
            deleteKey(key);
        }
    }

    int *leafKeys = (int*)malloc(sizeof(int) * M * 100);
    int size = 0;
    collectLeafKeys(root, leafKeys, &size, M * 100);

    qsort(leafKeys, size, sizeof(int), compare_int);

    for (int i = 0; i < size; i++) {
        printf("%d ", leafKeys[i]);
    }
    printf("\n");

    free(leafKeys);
    return 0;
}