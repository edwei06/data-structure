#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//2-3-4tree的insert和search
// B-Tree 階數 t=2 => 一個節點最多可以有 2t-1 = 3 個 key
#define MAX_KEYS 3
#define MIN_KEYS (MAX_KEYS / 2) // 在本例 t=2 時，最少要有 1 個 key

typedef struct BTreeNode {
    int keys[MAX_KEYS + 1];           // 儲存鍵值（多一格給「插入溢位」用）
    struct BTreeNode *children[MAX_KEYS + 2]; // 指向子節點
    int key_count;                    // 目前節點擁有多少個 key
    bool is_leaf;                     // 是否為葉子
} BTreeNode;

// 建立節點
BTreeNode* create_node(bool is_leaf) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->key_count = 0;
    node->is_leaf = is_leaf;
    for (int i = 0; i < MAX_KEYS + 2; i++) {
        node->children[i] = NULL;
    }
    return node;
}

// 分裂子節點
void split_child(BTreeNode *parent, int index, BTreeNode *child) {
    BTreeNode *right = create_node(child->is_leaf);

    int mid = (MAX_KEYS + 1) / 2; // 中間位置
    int pivot = child->keys[mid - 1]; // 中間的鍵

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

// 插入到非滿節點
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
        if (node->children[i]->key_count == MAX_KEYS) {
            split_child(node, i, node->children[i]);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insert_non_full(node->children[i], key);
    }
}

// 插入鍵值
BTreeNode* insert(BTreeNode *root, int key) {
    if (root->key_count == MAX_KEYS) {
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

// 按層級 (Level) BFS 列印
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

// 搜尋 B-Tree 的鍵值
bool search(BTreeNode *root, int key) {
    int i = 0;

    // 找到大於等於 key 的第一個鍵
    while (i < root->key_count && key > root->keys[i]) {
        i++;
    }

    // 如果鍵值在當前節點中，則返回 true
    if (i < root->key_count && key == root->keys[i]) {
        printf("Key %d found at node: [", key);
        for (int j = 0; j < root->key_count; j++) {
            printf("%d", root->keys[j]);
            if (j < root->key_count - 1) {
                printf(" ");
            }
        }
        printf("]\n");
        return true;
    }

    // 如果該節點是葉子，則找不到鍵值
    if (root->is_leaf) {
        return false;
    }

    // 向子節點遞迴搜尋
    return search(root->children[i], key);
}

int main() {
    BTreeNode *root = create_node(true);

    int keys[] = {8, 4, 2, 5, 10, 9, 6, 7};
    int n = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < n; i++) {
        root = insert(root, keys[i]);
    }

    print_levels(root);

    int search_keys[] = {5, 7, 1, 10};
    int m = sizeof(search_keys) / sizeof(search_keys[0]);

    for (int i = 0; i < m; i++) {
        if (!search(root, search_keys[i])) {
            printf("Key %d not found\n", search_keys[i]);
        }
    }

    return 0;
}
