#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
    
typedef struct BTreeNode {
    int *keys;           
    struct BTreeNode **children;
    int key_count;                   
    bool is_leaf;                    
} BTreeNode;

static int M;         
BTreeNode *root = NULL; 

BTreeNode* create_node(bool is_leaf) {
    BTreeNode *node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->keys = (int*)malloc((M+1) * sizeof(int));
    node->children = (BTreeNode**)malloc((M+2) * sizeof(BTreeNode*));
    node->key_count = 0;
    node->is_leaf = is_leaf;
    for (int i = 0; i < M + 2 ; i++) {
        node->children[i] = NULL;
    }
    return node;
}

int compare_int(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void split_child(BTreeNode *parent, int index, BTreeNode *child) {
    BTreeNode *right = create_node(child->is_leaf);

    int mid = (M + 1) / 2;
    int pivot = child->keys[mid - 1];

    right->key_count = child->key_count - mid;
    for (int i = 0; i < right->key_count; i++) {
        right->keys[i] = child->keys[mid + i];
    }

    if (!child->is_leaf) {
        for (int i = 0; i <= right->key_count; i++) {
            right->children[i] = child->children[mid + i];
        }
    }

    child->key_count = mid - 1;

    for (int i = parent->key_count; i > index; i--) {
        parent->keys[i] = parent->keys[i - 1];
        parent->children[i + 1] = parent->children[i];
    }
    parent->keys[index] = pivot;
    parent->children[index + 1] = right;
    parent->key_count++;
}

void insert_non_full(BTreeNode *node, int key) {
    int i = node->key_count - 1;
    if (node->is_leaf) {
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->key_count++;
    } else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->key_count == M) {
            split_child(node, i, node->children[i]);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insert_non_full(node->children[i], key);
    }
}

BTreeNode* insert(BTreeNode *root, int key) {
    if (root->key_count == M) {
        BTreeNode *new_root = create_node(false);
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

void print_levels(BTreeNode *root) {
    if (!root) return;
    BTreeNode* queue[100];
    int level[100];
    int front = 0, rear = 0;

    queue[rear] = root;
    level[rear++] = 1;

    int current_level = 1;
    printf("B-Tree Level Order Traversal:\n");
    printf("Level 1: ");

    while (front < rear) {
        BTreeNode* node = queue[front];
        int node_level = level[front++];
        if (node_level > current_level) {
            current_level = node_level;
            printf("\nLevel %d: ", current_level);
        }

        printf("[");
        for (int i = 0; i < node->key_count; i++) {
            printf("%d", node->keys[i]);
            if (i < node->key_count - 1) {
                printf(" ");
            }
        }
        printf("] ");

        if (!node->is_leaf) {
            for (int i = 0; i <= node->key_count; i++) {
                if (node->children[i]) {
                    queue[rear] = node->children[i];
                    level[rear++] = node_level + 1;
                }
            }
        }
    }
    printf("\n");
}

void collectLeafKeys(BTreeNode* node, int* arr, int* size, int maxSize) {
    if (!node) return;
    if (node->is_leaf) {
        for (int i = 0; i < node->key_count; i++) {
            arr[*size] = node->keys[i];
            (*size)++;
        }
    } else {
        for (int i = 0; i <= node->key_count; i++) {
            collectLeafKeys(node->children[i], arr, size, maxSize);
        }
    }
}

void removeFromLeaf(BTreeNode *node, int idx) {

    for (int i = idx; i < node->key_count - 1; i++) {
        node->keys[i] = node->keys[i + 1];
    }
    node->key_count--;
}


int findKeyPosition(BTreeNode *node, int key) {
    int idx = 0;
    while (idx < node->key_count && node->keys[idx] < key) {
        idx++;
    }
    if (idx < node->key_count && node->keys[idx] == key) {
        return idx;
    }
    return idx;
}


void deleteKeyInternal(BTreeNode *node, int key) {
    int idx = 0;

    while (idx < node->key_count && key > node->keys[idx]) {
        idx++;
    }

    if (node->is_leaf) {

        int pos = findKeyPosition(node, key);
        if (pos != -1) {
            removeFromLeaf(node, pos);
        }
        return;
    } else {
        if (idx < node->key_count && node->keys[idx] == key) {
            return;
        }
        deleteKeyInternal(node->children[idx], key);

        BTreeNode *child = node->children[idx];
        int minKeys = (M - 1) / 2; 
        if (child->key_count < minKeys) {
            BTreeNode *leftSibling = (idx - 1 >= 0) ? node->children[idx-1] : NULL;
            BTreeNode *rightSibling = (idx + 1 <= node->key_count) ? node->children[idx+1] : NULL;
            if (rightSibling && rightSibling->key_count > minKeys) {
                child->keys[child->key_count] = node->keys[idx]; 
                child->key_count++;
                node->keys[idx] = rightSibling->keys[0];
                for(int i=0; i < rightSibling->key_count - 1; i++){
                    rightSibling->keys[i] = rightSibling->keys[i+1];
                }
                rightSibling->key_count--;
            }
            else if (leftSibling && leftSibling->key_count > minKeys) {
                for(int i=child->key_count; i>0; i--){
                    child->keys[i] = child->keys[i-1];
                }
                child->keys[0] = node->keys[idx-1];
                child->key_count++;
                node->keys[idx-1] = leftSibling->keys[leftSibling->key_count-1];
                leftSibling->key_count--;
            }
            else {
                if (rightSibling) {
                    child->keys[child->key_count] = node->keys[idx];
                    child->key_count++;
                    for(int i=0; i<rightSibling->key_count; i++){
                        child->keys[child->key_count+i] = rightSibling->keys[i];
                    }
                    child->key_count += rightSibling->key_count;

                    for(int i=idx; i<node->key_count-1; i++){
                        node->keys[i] = node->keys[i+1];
                        node->children[i+1] = node->children[i+2];
                    }
                    node->key_count--;

                    free(rightSibling->keys);
                    free(rightSibling->children);
                    free(rightSibling);
                }
                else if (leftSibling) {
                    leftSibling->keys[leftSibling->key_count] = node->keys[idx-1];
                    leftSibling->key_count++;
                    for(int i=0; i<child->key_count; i++){
                        leftSibling->keys[leftSibling->key_count + i] = child->keys[i];
                    }
                    leftSibling->key_count += child->key_count;

                    for(int i=idx-1; i<node->key_count-1; i++){
                        node->keys[i] = node->keys[i+1];
                        node->children[i] = node->children[i+1];
                    }
                    node->children[node->key_count] = NULL;
                    node->key_count--;

                    free(child->keys);
                    free(child->children);
                    free(child);
                    child = leftSibling;
                }
            }
        }
    }
}

void delete(BTreeNode* root, int key) {
    if (!root) return; 

    deleteKeyInternal(root, key);
    if (root->key_count == 0 && !root->is_leaf) {
        BTreeNode *oldRoot = root;
        root = root->children[0];
        free(oldRoot->keys);
        free(oldRoot->children);
        free(oldRoot);
    }
}

int main() {
    BTreeNode *root = create_node(true);
    char command[32];
    int key;
    scanf("m-way %d", &M);

    while (1) {
        if (scanf("%s", command) != 1) break;

        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "insert") == 0) {
            scanf("%d", &key);
            root = insert(root, key);
        } else if (strcmp(command, "delete") == 0) {
            scanf("%d", &key);
            delete(root, key);
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