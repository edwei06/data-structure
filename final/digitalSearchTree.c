#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *key; 
    struct Node *left, *right; 
} Node;

Node *createNode(char *key) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->key = strdup(key); 
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

Node *insert(Node *root, char *key, int depth) {
    if (root == NULL) {
        return createNode(key);
    }

    // 根據鍵值的 bit 決定往左或往右
    if (key[depth] == '0') {
        root->left = insert(root->left, key, depth + 1);
    } else {
        root->right = insert(root->right, key, depth + 1);
    }

    return root;
}

int search(Node *root, char *key, int depth) {
    if (root == NULL) {
        return 0; // 沒找到
    }

    if (strcmp(root->key, key) == 0) {
        return 1; // 找到
    }

    // 根據鍵值的 bit 繼續搜尋
    if (key[depth] == '0') {
        return search(root->left, key, depth + 1);
    } else {
        return search(root->right, key, depth + 1);
    }
}

Node *deleteLeaf(Node *root) {
    Node *parent;
    while (root->left != NULL || root->right != NULL) {
        parent = root;
        if (root->left != NULL) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    if (parent->left == root)
    {
        parent->left = NULL;
    }else{
        parent->right = NULL;
    }
    return root;
}

Node *delete(Node *root, const char *key, int depth) {
    if (root == NULL) {
        return NULL;
    }

    if (strcmp(root->key, key) == 0) {
        // 如果左右子樹都不存在，直接返回 NULL
        if (root->left == NULL && root->right == NULL) {
            return NULL;
        }

        // 如果左右子樹都存在或只有其中一個存在
        Node *leaf = deleteLeaf(root);
        root->key = leaf->key; // 替換鍵值
        return root;
    }

    // 遞迴處理左右子樹
    if (key[depth] == '0') {
        root->left = delete(root->left, key, depth + 1);
    } else {
        root->right = delete(root->right, key, depth + 1);
    }

    return root;
}

void levelOrder(Node *root) {
    if (root == NULL) {
        return;
    }

    Node *queue[100]; // 假設樹不超過 100 個節點
    int front = 0, rear = 0;

    queue[rear++] = root;

    while (front < rear) {
        Node *current = queue[front++]; 

        printf("%s ", current->key);

        if (current->left != NULL) {
            queue[rear++] = current->left; 
        }

        if (current->right != NULL) {
            queue[rear++] = current->right;
        }
    }
}

int main() {
    Node *root = NULL;

    root = insert(root, "1000", 0);
    root = insert(root, "0010", 0);
    root = insert(root, "1001", 0);
    root = insert(root, "0001", 0);
    root = insert(root, "0000", 0);
    root = insert(root, "0011", 0);
    root = insert(root, "1101", 0);

    printf("Level-order traversal: ");
    levelOrder(root);
    printf("\n");

    printf("Search 0010: %s\n", search(root, "0010", 0) ? "Found" : "Not Found");

    root = delete(root, "0010", 0);
    printf("After deleting 0010, level-order traversal: ");
    levelOrder(root);
    printf("\n");

    return 0;
}
