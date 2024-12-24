#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

typedef struct FibNode {
    int key;
    int degree;
    bool mark;
    struct FibNode* parent;
    struct FibNode* child;
    struct FibNode* left;
    struct FibNode* right;
} FibNode;

typedef struct FibHeap {
    FibNode* min;
    int n;
} FibHeap;

FibNode* createNode(int key) {
    FibNode* node = (FibNode*)malloc(sizeof(FibNode));
    node->key = key;
    node->degree = 0;
    node->mark = false;
    node->parent = NULL;
    node->child = NULL;
    node->left = node;
    node->right = node;
    return node;
}

FibHeap* createFibHeap() {
    FibHeap* H = (FibHeap*)malloc(sizeof(FibHeap));
    H->min = NULL;
    H->n = 0;
    return H;
}

void fibHeapMergeLists(FibNode* a, FibNode* b) {
    if (!a || !b) return;
    FibNode* tempA = a->right;
    FibNode* tempB = b->right;
    a->right = tempB;
    tempB->left = a;
    b->right = tempA;
    tempA->left = b;
}

void fibHeapInsert(FibHeap* H, FibNode* x) {
    if (!H->min) {
        H->min = x;
    } else {
        x->left = H->min->left;
        x->right = H->min;
        H->min->left->right = x;
        H->min->left = x;
        if (x->key < H->min->key) {
            H->min = x;
        }
    }
    H->n++;
}

FibNode* fibHeapFindNode(FibNode* current, int key) {
    if (!current) return NULL;
    FibNode* start = current;
    FibNode* found = NULL;
    bool done = false;
    while (!done) {
        if (current->key == key) {
            found = current;
            break;
        }
        if (current->child) {
            found = fibHeapFindNode(current->child, key);
            if (found) break;
        }
        current = current->right;
        done = (current == start);
    }
    return found;
}

/* --- Corrected child-list sorting: collect children in array, sort by key, re-link. --- */
void sortChildListAscending(FibNode* parent) {
    if (!parent->child || parent->child->right == parent->child) return;
    FibNode* start = parent->child;
    FibNode* c = start;
    int count = 0;
    bool firstPass = true;
    // Collect children in array
    FibNode* arr[200];
    while (firstPass || c != start) {
        firstPass = false;
        arr[count++] = c;
        c = c->right;
    }
    // Sort by ascending key
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[i]->key > arr[j]->key) {
                FibNode* tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
    // Re-link in ascending order
    for (int i = 0; i < count; i++) {
        arr[i]->left = arr[(i - 1 + count) % count];
        arr[i]->right = arr[(i + 1) % count];
    }
    parent->child = arr[0];
}

/* Make y child of x */
void fibHeapLink(FibHeap* H, FibNode* y, FibNode* x) {
    y->left->right = y->right;
    y->right->left = y->left;
    y->parent = x;
    y->mark = false;
    if (!x->child) {
        x->child = y;
        y->left = y;
        y->right = y;
    } else {
        y->left = x->child->left;
        y->right = x->child;
        x->child->left->right = y;
        x->child->left = y;
    }
    x->degree++;
    sortChildListAscending(x);
}

void fibHeapConsolidate(FibHeap* H) {
    if (!H->min) return;
    int maxDegree = 50;
    FibNode* A[50];
    for (int i = 0; i < maxDegree; i++) A[i] = NULL;
    FibNode* roots[200];
    int numRoots = 0;
    FibNode* start = H->min;
    FibNode* w = H->min;
    bool done = false;
    while (!done) {
        roots[numRoots++] = w;
        w = w->right;
        done = (w == start);
    }
    for (int i = 0; i < numRoots; i++) {
        FibNode* x = roots[i];
        int d = x->degree;
        while (A[d]) {
            FibNode* y = A[d];
            if (x->key > y->key) {
                FibNode* temp = x; x = y; y = temp;
            }
            fibHeapLink(H, y, x);
            A[d] = NULL;
            d++;
        }
        A[d] = x;
    }
    H->min = NULL;
    for (int i = 0; i < maxDegree; i++) {
        if (A[i]) {
            A[i]->left = A[i];
            A[i]->right = A[i];
            if (!H->min) {
                H->min = A[i];
            } else {
                fibHeapMergeLists(H->min, A[i]);
                if (A[i]->key < H->min->key) {
                    H->min = A[i];
                }
            }
        }
    }
}

FibNode* fibHeapExtractMin(FibHeap* H) {
    FibNode* z = H->min;
    if (z) {
        if (z->child) {
            FibNode* startChild = z->child;
            FibNode* c = z->child;
            bool firstPass = true;
            while (firstPass || c != startChild) {
                firstPass = false;
                FibNode* nxt = c->right;
                c->parent = NULL;
                c->left->right = c->right;
                c->right->left = c->left;
                c->left = H->min->left;
                c->right = H->min;
                H->min->left->right = c;
                H->min->left = c;
                c = nxt;
            }
        }
        z->left->right = z->right;
        z->right->left = z->left;
        if (z == z->right) {
            H->min = NULL;
        } else {
            H->min = z->right;
            fibHeapConsolidate(H);
        }
        H->n--;
    }
    return z;
}

void fibHeapCut(FibHeap* H, FibNode* x, FibNode* y) {
    if (x->right == x) {
        y->child = NULL;
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x) y->child = x->right;
    }
    y->degree--;
    x->parent = NULL;
    x->mark = false;
    x->left = H->min->left;
    x->right = H->min;
    H->min->left->right = x;
    H->min->left = x;
    if (x->key < H->min->key) {
        H->min = x;
    }
}

void fibHeapCascadingCut(FibHeap* H, FibNode* y) {
    FibNode* z = y->parent;
    if (z) {
        if (!y->mark) {
            y->mark = true;
        } else {
            fibHeapCut(H, y, z);
            fibHeapCascadingCut(H, z);
        }
    }
}

void fibHeapDecreaseKey(FibHeap* H, FibNode* x, int newKey) {
    if (!x) return;
    if (newKey >= x->key) return;
    x->key = newKey;
    FibNode* y = x->parent;
    if (y && x->key < y->key) {
        fibHeapCut(H, x, y);
        fibHeapCascadingCut(H, y);
    }
    if (x->key < H->min->key) {
        H->min = x;
    }
}

void fibHeapDelete(FibHeap* H, FibNode* x) {
    if (!x) return;
    fibHeapDecreaseKey(H, x, INT_MIN);
    FibNode* removed = fibHeapExtractMin(H);
    if (removed) free(removed);
}

/* ----- Level-order printing (each tree on its own line) ----- */

typedef struct QueueNode {
    FibNode* node;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* rear;
} Queue;

void initQueue(Queue* q) {
    q->front = q->rear = NULL;
}

bool isEmptyQueue(Queue* q) {
    return (q->front == NULL);
}

void enqueue(Queue* q, FibNode* node) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->node = node;
    newNode->next = NULL;
    if (q->rear) q->rear->next = newNode;
    else q->front = newNode;
    q->rear = newNode;
}

FibNode* dequeue(Queue* q) {
    if (isEmptyQueue(q)) return NULL;
    QueueNode* temp = q->front;
    FibNode* node = temp->node;
    q->front = q->front->next;
    if (!q->front) q->rear = NULL;
    free(temp);
    return node;
}

void printTreeLevelOrder(FibNode* root) {
    if (!root) return;
    Queue q;
    initQueue(&q);
    enqueue(&q, root);
    bool visited[1000] = {false}; 
    char buffer[1024]; 
    buffer[0] = '\0';
    char temp[50];

    while (!isEmptyQueue(&q)) {
        FibNode* curr = dequeue(&q);
        sprintf(temp, "%d ", curr->key);
        strcat(buffer, temp);
        if (curr->child) {
            FibNode* startChild = curr->child;
            FibNode* c = startChild;
            bool firstPass = true;
            while (firstPass || c != startChild) {
                firstPass = false;
                enqueue(&q, c);
                c = c->right;
            }
        }
    }
    int len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == ' ') buffer[len - 1] = '\0';
    printf("%s\n", buffer);
}

void printLevelOrder(FibHeap* H) {
    if (!H->min) return;
    FibNode* roots[200];
    int count = 0;
    FibNode* start = H->min;
    FibNode* w = H->min;
    bool done = false;
    while (!done) {
        roots[count++] = w;
        w = w->right;
        done = (w == start);
    }
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (roots[i]->degree > roots[j]->degree ||
               (roots[i]->degree == roots[j]->degree && roots[i]->key > roots[j]->key)) {
                FibNode* tmp = roots[i];
                roots[i] = roots[j];
                roots[j] = tmp;
            }
        }
    }
    for (int i = 0; i < count; i++) {
        printTreeLevelOrder(roots[i]);
    }
}

int main() {
    FibHeap* heap = createFibHeap();
    char command[20];
    int key, value;

    while (1) {
        if (scanf("%s", command) == EOF) break;
        if (!strcmp(command, "insert")) {
            scanf("%d", &key);
            FibNode* node = createNode(key);
            fibHeapInsert(heap, node);
        } else if (!strcmp(command, "delete")) {
            scanf("%d", &key);
            FibNode* node = fibHeapFindNode(heap->min, key);
            fibHeapDelete(heap, node);
        } else if (!strcmp(command, "decrease")) {
            scanf("%d %d", &key, &value);
            FibNode* node = fibHeapFindNode(heap->min, key);
            if (node) {
                int newKey = node->key - value;
                fibHeapDecreaseKey(heap, node, newKey);
            }
        } else if (!strcmp(command, "extract-min")) {
            FibNode* minNode = fibHeapExtractMin(heap);
            if (minNode) free(minNode);
        } else if (!strcmp(command, "exit")) {
            break;
        } else {
            break;
        }
    }

    printLevelOrder(heap);
    return 0;
}
