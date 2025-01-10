#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN_DEGREE 2 // Minimum degree (defines the range for number of keys)

typedef struct Node {
    int* keys;
    int t;
    struct Node** children;
    int n;
    bool leaf;
    struct Node* next;
} Node;

typedef struct BTree {
    Node* root;
    int t;
} BTree;

Node* createNode(int t, bool leaf)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->t = t;
    newNode->leaf = leaf;
    newNode->keys = (int*)malloc((2 * t - 1) * sizeof(int));
    newNode->children = (Node**)malloc((2 * t) * sizeof(Node*));
    newNode->n = 0;
    newNode->next = NULL;
    return newNode;
}

BTree* createBTree(int t)
{
    BTree* btree = (BTree*)malloc(sizeof(BTree));
    btree->t = t;
    btree->root = createNode(t, true);
    return btree;
}

void levelOrder(Node* root)
{
    if (!root) return;

    Node** queue = (Node**)malloc(1000 * sizeof(Node*));
    int front = 0, rear = 0;

    queue[rear++] = root;
    int level = 0;

    while (front < rear) {
        int levelSize = rear - front;

        printf("Level %d: ", level);
        level++;

        for (int i = 0; i < levelSize; i++) {
            Node* current = queue[front++];

            printf("[");
            for (int j = 0; j < current->n; j++) {
                printf("%d", current->keys[j]);
                if (j < current->n - 1) printf(", ");
            }
            printf("] ");

            if (!current->leaf) {
                for (int j = 0; j <= current->n; j++) {
                    queue[rear++] = current->children[j];
                }
            }
        }

        printf("\n");
    }

    free(queue);
}

void display(Node* node)
{
    if (node == NULL) return;

    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf) {
            display(node->children[i]);
        }
        printf("%d ", node->keys[i]);
    }
    if (!node->leaf) {
        display(node->children[i]);
    }
}

bool search(Node* node, int key)
{
    int i = 0;
    while (i < node->n && key > node->keys[i]) {
        i++;
    }
    if (i < node->n && key == node->keys[i]) {
        return true;
    }
    if (node->leaf) {
        return false;
    }
    return search(node->children[i], key);
}

void splitChild(Node* parent, int i, Node* child)
{
    int t = child->t;
    Node* newChild = createNode(t, child->leaf);
    newChild->n = t - 1;

    for (int j = 0; j < t - 1; j++) {
        newChild->keys[j] = child->keys[j + t];
    }

    if (!child->leaf) {
        for (int j = 0; j < t; j++) {
            newChild->children[j] = child->children[j + t];
        }
    }

    child->n = t - 1;

    for (int j = parent->n; j >= i + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = newChild;

    for (int j = parent->n - 1; j >= i; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }
    parent->keys[i] = child->keys[t - 1];
    parent->n += 1;
}

void insertNonFull(Node* node, int key)
{
    int i = node->n - 1;

    if (node->leaf) {
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->n += 1;
    } else {
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;
        if (node->children[i]->n == 2 * node->t - 1) {
            splitChild(node, i, node->children[i]);
            if (node->keys[i] < key) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

void insert(BTree* btree, int key)
{
    Node* root = btree->root;
    if (root->n == 2 * btree->t - 1) {
        Node* newRoot = createNode(btree->t, false);
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);
        insertNonFull(newRoot, key);
        btree->root = newRoot;
    } else {
        insertNonFull(root, key);
    }
}

void deleteKeyHelper(Node* node, int key);
int findKey(Node* node, int key);
void removeFromLeaf(Node* node, int idx);
int getPredecessor(Node* node, int idx);
void fill(Node* node, int idx);
void borrowFromPrev(Node* node, int idx);
void borrowFromNext(Node* node, int idx);
void merge(Node* node, int idx);

void deleteKey(BTree* btree, int key)
{
    Node* root = btree->root;

    deleteKeyHelper(root, key);

    if (root->n == 0 && !root->leaf) {
        btree->root = root->children[0];
        free(root);
    }
}

void deleteKeyHelper(Node* node, int key)
{
    int idx = findKey(node, key);

    if (idx < node->n && node->keys[idx] == key) {
        if (node->leaf) {
            removeFromLeaf(node, idx);
        } else {
            int predecessor = getPredecessor(node, idx);
            node->keys[idx] = predecessor;
            deleteKeyHelper(node->children[idx], predecessor);
        }
    } else {
        if (node->leaf) {
            printf("Key %d not found in the B+ tree.\n", key);
            return;
        }

        bool isLastChild = (idx == node->n);

        if (node->children[idx]->n < node->t) {
            fill(node, idx);
        }

        if (isLastChild && idx > node->n) {
            deleteKeyHelper(node->children[idx - 1], key);
        } else {
            deleteKeyHelper(node->children[idx], key);
        }
    }
}

int findKey(Node* node, int key)
{
    int idx = 0;
    while (idx < node->n && key > node->keys[idx]) {
        idx++;
    }
    return idx;
}

void removeFromLeaf(Node* node, int idx)
{
    for (int i = idx + 1; i < node->n; ++i) {
        node->keys[i - 1] = node->keys[i];
    }
    node->n--;
}

int getPredecessor(Node* node, int idx)
{
    Node* curr = node->children[idx];
    while (!curr->leaf) {
        curr = curr->children[curr->n];
    }
    return curr->keys[curr->n - 1];
}

void fill(Node* node, int idx)
{
    if (idx != 0 && node->children[idx - 1]->n >= node->t) {
        borrowFromPrev(node, idx);
    } else if (idx != node->n && node->children[idx + 1]->n >= node->t) {
        borrowFromNext(node, idx);
    } else {
        if (idx != node->n) {
            merge(node, idx);
        } else {
            merge(node, idx - 1);
        }
    }
}

void borrowFromPrev(Node* node, int idx)
{
    Node* child = node->children[idx];
    Node* sibling = node->children[idx - 1];

    for (int i = child->n - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
    }

    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i) {
            child->children[i + 1] = child->children[i];
        }
    }

    child->keys[0] = node->keys[idx - 1];

    if (!child->leaf) {
        child->children[0] = sibling->children[sibling->n];
    }

    node->keys[idx - 1] = sibling->keys[sibling->n - 1];

    child->n += 1;
    sibling->n -= 1;
}

void borrowFromNext(Node* node, int idx)
{
    Node* child = node->children[idx];
    Node* sibling = node->children[idx + 1];

    child->keys[(child->n)] = node->keys[idx];

    if (!child->leaf) {
        child->children[(child->n) + 1] = sibling->children[0];
    }

    node->keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->n; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
    }

    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }

    child->n += 1;
    sibling->n -= 1;
}

void merge(Node* node, int idx)
{
    Node* child = node->children[idx];
    Node* sibling = node->children[idx + 1];

    child->keys[child->n] = node->keys[idx];

    if (!child->leaf) {
        child->children[child->n + 1] = sibling->children[0];
    }

    for (int i = 0; i < sibling->n; ++i) {
        child->keys[i + child->n + 1] = sibling->keys[i];
    }

    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i) {
            child->children[i + child->n + 1] = sibling->children[i];
        }
    }

    for (int i = idx + 1; i < node->n; ++i) {
        node->keys[i - 1] = node->keys[i];
    }

    for (int i = idx + 2; i <= node->n; ++i) {
        node->children[i - 1] = node->children[i];
    }

    child->n += sibling->n + 1;
    node->n--;

    free(sibling);
}

int main()
{
    BTree* btree = createBTree(MIN_DEGREE);

    insert(btree, 3);
    insert(btree, 2);
    insert(btree, 4);
    deleteKey(btree, 3);

    printf("B+ Tree (Level Order):\n");
    levelOrder(btree->root);

    return 0;
}