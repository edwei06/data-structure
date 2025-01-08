#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct FibNode {
    int key;                    
    int degree;              
    int mark;          
    
    struct FibNode* parent;  
    struct FibNode* child;
    struct FibNode* left;
    struct FibNode* right;
} FibNode;

typedef struct FibHeap {
    FibNode* min;
    int n;
} FibHeap;


FibHeap* createFibHeap();
FibNode* createFibNode(int key);
void fibInsert(FibHeap* H, int key);
void fibInsertNode(FibHeap* H, FibNode* x);
void fibLink(FibHeap* H, FibNode* y, FibNode* x);
void consolidate(FibHeap* H);
FibNode* fibExtractMin(FibHeap* H);
void fibCut(FibHeap* H, FibNode* x, FibNode* y);
void fibCascadingCut(FibHeap* H, FibNode* y);
void fibDecreaseKey(FibHeap* H, int oldKey, int decVal);
void fibDelete(FibHeap* H, int key);
FibNode* searchKeyInRootList(FibNode* min, int key);
FibNode* searchKey(FibNode* x, int key);
void linkChildrenSorted(FibNode* parent, FibNode* child); // keep children sorted
void printFibonacciHeap(FibHeap* H);

FibHeap* createFibHeap() {
    FibHeap* H = (FibHeap*) malloc(sizeof(FibHeap));
    H->min = NULL;
    H->n = 0;
    return H;
}

FibNode* createFibNode(int key) {
    FibNode* node = (FibNode*) malloc(sizeof(FibNode));
    node->key = key;
    node->degree = 0;
    node->mark = 0;
    node->parent = NULL;
    node->child = NULL;
    node->left = node;
    node->right = node;
    return node;
}

void fibInsert(FibHeap* H, int key) {
    FibNode* node = createFibNode(key);
    fibInsertNode(H, node);
}

void fibInsertNode(FibHeap* H, FibNode* x) {
    if (H->min == NULL) {
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

void fibLink(FibHeap* H, FibNode* y, FibNode* x) {
    y->left->right = y->right;
    y->right->left = y->left;
    y->parent = x;
    y->mark = 0;
    linkChildrenSorted(x, y);

    x->degree++;
}

void linkChildrenSorted(FibNode* parent, FibNode* child) {
    if (parent->child == NULL) {
        parent->child = child;
        child->left = child;
        child->right = child;
    } else {
        FibNode* start = parent->child;
        FibNode* curr = start;
        do {
            if (child->key < curr->key) {
                child->left = curr->left;
                child->right = curr;
                curr->left->right = child;
                curr->left = child;
                
                // If we inserted before the head, update parent's child pointer
                if (curr == start && child->key < start->key) {
                    parent->child = child;
                }
                return;
            }
            curr = curr->right;
        } while (curr != start);
        // If we reach here, 'child' is >= every node in the child list
        // Insert child after the last node
        child->left = start->left;
        child->right = start;
        start->left->right = child;
        start->left = child;
    }
}

/*
 * CONSOLIDATE: Merge roots of the same degree until at most one root
 * of each degree remains.
 * According to the problem statement, if multiple roots have the same degree,
 * “the two roots with the smallest key values are merged first”.
 */
void consolidate(FibHeap* H) {
    if (H->min == NULL) return;

    FibNode* start = H->min;
    FibNode* w = start;
    int numRoots = 0;
    do {
        numRoots++;
        w = w->right;
    } while (w != start);

    FibNode** rootList = (FibNode**)malloc(numRoots * sizeof(FibNode*));
    w = start;
    for (int i = 0; i < numRoots; i++) {
        rootList[i] = w;
        w = w->right;
    }

    for (int i = 0; i < numRoots - 1; i++) {
        for (int j = i + 1; j < numRoots; j++) {
            if (rootList[i]->key > rootList[j]->key) {
                FibNode* tmp = rootList[i];
                rootList[i] = rootList[j];
                rootList[j] = tmp;
            }
        }
    }

    int maxDegree = 0;
    while ((1 << maxDegree) <= H->n) {
        maxDegree++;
    }
    FibNode** A = (FibNode**)calloc(maxDegree + 1, sizeof(FibNode*));
    for (int i = 0; i <= maxDegree; i++) {
        A[i] = NULL;
    }

    for (int i = 0; i < numRoots; i++) {
        FibNode* x = rootList[i];
        if (!x) continue;

        int d = x->degree;
        while (A[d] != NULL) {
            FibNode* y = A[d];
            if (y->key < x->key) {
                FibNode* tmp = x;
                x = y;
                y = tmp;
            }
            fibLink(H, y, x);
            A[d] = NULL;
            d++;
        }
        A[d] = x;
    }

    // Reconstruct the root list and find new min
    H->min = NULL;
    for (int i = 0; i <= maxDegree; i++) {
        if (A[i]) {
            // Isolate A[i] so we can insert it cleanly
            A[i]->left = A[i];
            A[i]->right = A[i];
            A[i]->parent = NULL;

            if (H->min == NULL) {
                // First root
                H->min = A[i];
            } else {
                // Insert into root list
                A[i]->left = H->min->left;
                A[i]->right = H->min;
                H->min->left->right = A[i];
                H->min->left = A[i];
                // Update min if needed
                if (A[i]->key < H->min->key) {
                    H->min = A[i];
                }
            }
        }
    }
    free(A);
    free(rootList);
}



// EXTRACT-MIN: removes the min root and merges its children into the root list, then consolidates
FibNode* fibExtractMin(FibHeap* H) {
    FibNode* z = H->min;
    if (z != NULL) {
        // For each child of z, add it to the root list
        FibNode* child = z->child;
        if (child != NULL) {
            FibNode* start = child;
            do {
                FibNode* next = child->right;
                // Add child to root list
                child->parent = NULL;
                // Insert child in root list next to z
                child->left = z->left;
                child->right = z;
                z->left->right = child;
                z->left = child;
                child = next;
            } while (child != start);
        }
        
        // Remove z from root list
        z->left->right = z->right;
        z->right->left = z->left;
        
        if (z == z->right) {
            // z was the only node in the root list
            H->min = NULL;
        } else {
            H->min = z->right;
            consolidate(H);
        }
        H->n--;
    }
    return z;
}

// CUT: remove x from y's child list and add x to the root list 
void fibCut(FibHeap* H, FibNode* x, FibNode* y) {
    // Remove x from y's child list
    if (x->right == x) {
        // x is the only child
        y->child = NULL;
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
        // if x was the pointer y->child, update it
        if (y->child == x) {
            y->child = x->right;
        }
    }
    y->degree--;
    // Add x to root list
    x->parent = NULL;
    x->mark = 0;
    x->left = H->min->left;
    x->right = H->min;
    H->min->left->right = x;
    H->min->left = x;
    // Update min if needed
    if (x->key < H->min->key) {
        H->min = x;
    }
}

// CASCADING-CUT: cut y from its parent if needed, and so on
void fibCascadingCut(FibHeap* H, FibNode* y) {
    FibNode* z = y->parent;
    if (z != NULL) {
        if (y->mark == 0) {
            y->mark = 1;
        } else {
            fibCut(H, y, z);
            fibCascadingCut(H, z);
        }
    }
}

// SEARCH: search for a key in the root list (and children)
// We do a BFS/DFS approach to find the node with a given key.
FibNode* searchKeyInRootList(FibNode* min, int key) {
    if (!min) return NULL;
    // Each tree in the root list
    FibNode* current = min;
    do {
        FibNode* found = searchKey(current, key);
        if (found != NULL) return found;
        current = current->right;
    } while (current != min);
    return NULL;
}

// Recursive search in a tree rooted at x
FibNode* searchKey(FibNode* x, int key) {
    if (x == NULL) return NULL;
    FibNode* start = x;
    FibNode* current = x;
    do {
        if (current->key == key) {
            return current;
        }
        // search children
        FibNode* result = searchKey(current->child, key);
        if (result) return result;
        current = current->right;
    } while (current != start);
    return NULL;
}

/*
 * DECREASE-KEY: reduce 'oldKey' by 'decVal' => newKey = oldKey - decVal
 * Assumes oldKey exists and decVal <= oldKey so final key is >= 0
 */
void fibDecreaseKey(FibHeap* H, int oldKey, int decVal) {
    FibNode* x = searchKeyInRootList(H->min, oldKey);
    if (!x) return; // should not happen in valid usage
    
    int newKey = x->key - decVal;
    x->key = newKey;
    FibNode* y = x->parent;
    if (y != NULL && x->key < y->key) {
        fibCut(H, x, y);
        fibCascadingCut(H, y);
    }
    // Update min
    if (x->key < H->min->key) {
        H->min = x;
    }
}

/* DELETE a key from the Fibonacci heap:
 * 1) Decrease the key to -∞
 * 2) Extract-min
 */
void fibDelete(FibHeap* H, int key) {
    FibNode* x = searchKeyInRootList(H->min, key);
    if (!x) return; // should not happen in valid usage

    // Decrease key to something smaller than any existing key (simulate -∞)
    x->key = INT_MIN;
    FibNode* y = x->parent;
    if (y != NULL) {
        fibCut(H, x, y);
        fibCascadingCut(H, y);
    }
    // Now x is in the root list and definitely is the min
    H->min = x;
    // extract-min
    fibExtractMin(H);
}

#include <stdbool.h>
typedef struct QueueNode {
    FibNode* node;
    int level;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* rear;
} Queue;

void enqueue(Queue* q, FibNode* node, int level) {
    if (!node) return;
    QueueNode* tmp = (QueueNode*)malloc(sizeof(QueueNode));
    tmp->node = node;
    tmp->level = level;
    tmp->next = NULL;
    if (!q->rear) {
        q->front = q->rear = tmp;
    } else {
        q->rear->next = tmp;
        q->rear = tmp;
    }
}

bool dequeue(Queue* q, FibNode** outNode, int* outLevel) {
    if (!q->front) return false;
    QueueNode* tmp = q->front;
    *outNode = tmp->node;
    *outLevel = tmp->level;
    q->front = tmp->next;
    if (!q->front) q->rear = NULL;
    free(tmp);
    return true;
}

/* BFS print for a single tree (rooted at 'root'). */
void printTreeLevelOrder(FibNode* root) {
    if (!root) return;
    Queue q = {0};
    enqueue(&q, root, 0);
    int currentLevel = 0;
    
    while (true) {
        FibNode* node;
        int level;
        if (!dequeue(&q, &node, &level)) break;
        // If we reached a new level, print a new line
        if (level > currentLevel) {
            // end of previous level
            currentLevel = level;
            printf(" ");
        }
        printf("%d ", node->key);
        if (node->child) {
            FibNode* cstart = node->child;
            FibNode* c = cstart;
            do {
                enqueue(&q, c, level + 1);
                c = c->right;
            } while (c != cstart);
        }
    }
}
void printFibonacciHeap(FibHeap* H) {
    if (!H->min) return;
    
    // Collect root nodes in an array
    FibNode* start = H->min;
    FibNode* r = start;
    int rootCount = 0;
    do {
        rootCount++;
        r = r->right;
    } while (r != start);
    
    FibNode** roots = (FibNode**)malloc(rootCount * sizeof(FibNode*));
    r = start;
    for (int i = 0; i < rootCount; i++) {
        roots[i] = r;
        r = r->right;
    }
    // Sort the roots by degree (ascending)
    for (int i = 0; i < rootCount - 1; i++) {
        for (int j = i+1; j < rootCount; j++) {
            if (roots[i]->degree > roots[j]->degree) {
                FibNode* tmp = roots[i];
                roots[i] = roots[j];
                roots[j] = tmp;
            }
        }
    }
    
    // For each root, print its BFS in a single line
    for (int i = 0; i < rootCount; i++) {
        Queue q = {0};
        enqueue(&q, roots[i], 0);
        int prevLevel = 0;
        while (true) {
            FibNode* node;
            int level;
            if (!dequeue(&q, &node, &level)) break;
            
            // Print the node's key (same line).
            printf("%d ", node->key);
            
            // Enqueue children
            if (node->child) {
                FibNode* cstart = node->child;
                FibNode* c = cstart;
                do {
                    enqueue(&q, c, level + 1);
                    c = c->right;
                } while (c != cstart);
            }
        }
        printf("\n");  // end of this tree
    }
    
    free(roots);
}

int main() {
    FibHeap* heap = createFibHeap();
    
    char command[32];
    int key, value;
    
    while (1) {
        if (scanf("%s", command) != 1) {
            break; // no more input
        }
        
        if (strcmp(command, "insert") == 0) {
            scanf("%d", &key);
            fibInsert(heap, key);
        }
        else if (strcmp(command, "delete") == 0) {
            scanf("%d", &key);
            fibDelete(heap, key);
        }
        else if (strcmp(command, "decrease") == 0) {
            scanf("%d %d", &key, &value);
            fibDecreaseKey(heap, key, value);
        }
        else if (strcmp(command, "extract-min") == 0) {
            FibNode* minNode = fibExtractMin(heap);
            if (minNode) {
                free(minNode); // we don't need minNode after extraction
            }
        }
        else if (strcmp(command, "exit") == 0) {
            break;
        }
        else {
            break;
        }
    }
    printFibonacciHeap(heap);
    return 0;
}
