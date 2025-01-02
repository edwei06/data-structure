#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* --------------- DATA STRUCTURES --------------- */

/* A node in the Fibonacci heap */
typedef struct FibNode {
    int key;                    // value (unique in the heap)
    int degree;                 // number of children
    int mark;                   // for cascading cuts (0 or 1)
    
    struct FibNode* parent;     // parent pointer
    struct FibNode* child;      // pointer to any child in circular list of children
    struct FibNode* left;       // left sibling (circular)
    struct FibNode* right;      // right sibling (circular)
} FibNode;

/* The Fibonacci Heap itself */
typedef struct FibHeap {
    FibNode* min;              // pointer to the root with minimum key
    int n;                     // total number of nodes in the heap
} FibHeap;

/* --------------- FUNCTION PROTOTYPES --------------- */

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

/* --------------- HELPER FUNCTIONS --------------- */

/* Create an empty Fibonacci heap */
FibHeap* createFibHeap() {
    FibHeap* H = (FibHeap*) malloc(sizeof(FibHeap));
    H->min = NULL;
    H->n = 0;
    return H;
}

/* Create a single Fibonacci node */
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

/* Insert a key into the Fibonacci heap */
void fibInsert(FibHeap* H, int key) {
    FibNode* node = createFibNode(key);
    fibInsertNode(H, node);
}

/* Insert an already-created node into the root list of the Fibonacci heap */
void fibInsertNode(FibHeap* H, FibNode* x) {
    if (H->min == NULL) {
        // Heap is empty, new node is min
        H->min = x;
    } else {
        // Insert x into root list to the left of H->min
        x->left = H->min->left;
        x->right = H->min;
        H->min->left->right = x;
        H->min->left = x;
        // Update min if necessary
        if (x->key < H->min->key) {
            H->min = x;
        }
    }
    H->n++;
}

/* LINK: Make y a child of x; x and y must have the same degree */
void fibLink(FibHeap* H, FibNode* y, FibNode* x) {
    // Remove y from root list
    y->left->right = y->right;
    y->right->left = y->left;
    y->parent = x;
    y->mark = 0;

    // Insert y into x's child list (keep children sorted by key)
    linkChildrenSorted(x, y);

    x->degree++;
}

/* Insert the child into the parent's child list, keeping them sorted by key */
void linkChildrenSorted(FibNode* parent, FibNode* child) {
    if (parent->child == NULL) {
        // No existing children, child is the only child
        parent->child = child;
        child->left = child;
        child->right = child;
    } else {
        // Insert into child-list in sorted order by key
        FibNode* start = parent->child;
        FibNode* curr = start;
        
        // We will insert 'child' in ascending order. Because we keep a
        // circular list, let's find the correct position.
        do {
            if (child->key < curr->key) {
                // Insert child before curr
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
    
    // Upper bound on degree: for n nodes, degree <= log2(n) base phi, but we'll
    // pick something safe for small n. 
    int maxDegree = 0;
    // a small over-estimation for simplicity
    while ((1 << maxDegree) <= H->n) {
        maxDegree++;
    }

    FibNode** A = (FibNode**) calloc(maxDegree+1, sizeof(FibNode*));
    for (int i = 0; i <= maxDegree; i++) {
        A[i] = NULL;
    }

    // Collect all roots into an array
    FibNode* start = H->min;
    FibNode* w = H->min;
    int numRoots = 0;
    
    // Count how many roots we have
    do {
        numRoots++;
        w = w->right;
    } while (w != start);

    // We need to iterate over each root. We can traverse again carefully,
    // storing them in an array so the linking does not break our iteration.
    FibNode** rootList = (FibNode**) malloc(numRoots * sizeof(FibNode*));

    w = start;
    for (int i = 0; i < numRoots; i++) {
        rootList[i] = w;
        w = w->right;
    }

    // For each root in the root list, link if necessary
    for (int i = 0; i < numRoots; i++) {
        FibNode* x = rootList[i];
        int d = x->degree;
        // x could be changed by linking, so we store it again
        while (A[d] != NULL) {
            FibNode* y = A[d];
            // Link the two with the smaller key as the parent
            // "the two roots with the smallest key values are merged first"
            // means we should link the pair in ascending order of keys
            if (y->key < x->key) {
                // swap
                FibNode* temp = x;
                x = y;
                y = temp;
            }
            // y is linked under x
            fibLink(H, y, x);
            A[d] = NULL;
            d++;
        }
        A[d] = x;
    }
    free(rootList);

    // Reconstruct the root list from A and find new min
    H->min = NULL;
    for (int i = 0; i <= maxDegree; i++) {
        if (A[i] != NULL) {
            // Insert A[i] into the root list
            if (H->min == NULL) {
                H->min = A[i];
                H->min->left = H->min;
                H->min->right = H->min;
            } else {
                // Insert into root list
                A[i]->left = H->min->left;
                A[i]->right = H->min;
                H->min->left->right = A[i];
                H->min->left = A[i];
                if (A[i]->key < H->min->key) {
                    H->min = A[i];
                }
            }
        }
    }
    free(A);
}

/* EXTRACT-MIN: removes the min root and merges its children into the root list, then consolidates */
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

/* CUT: remove x from y's child list and add x to the root list */
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

/* CASCADING-CUT: cut y from its parent if needed, and so on */
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

/* SEARCH: search for a key in the root list (and children) */
/* We do a BFS/DFS approach to find the node with a given key. */
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

/* Recursive search in a tree rooted at x */
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

/* --------------- PRINT (LEVEL-ORDER) --------------- */

/*
 * Print the final Fibonacci Heap structure in level-order traversal.
 * Per the problem statement:
 *    - Each root's tree is printed in a separate line.
 *    - Within the same level of a tree, nodes are printed from left to right.
 *    - The root list itself is traversed in ascending order of degrees, but
 *      typically we'll just store them in the order they appear after
 *      all operations are done. The problem states: "Start with the tree of a smaller degree".
 *      We can do a stable approach by simply sorting the root list by degree.
 */

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
    
    // We do BFS in a single circular tree
    // The children form their own circular lists, etc.
    
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
        
        // Print the current node
        printf("%d ", node->key);
        
        // Enqueue all siblings in this level (besides the node itself),
        // but only if it’s the "first" in that sibling set.
        // Actually for BFS in a tree, typically we only enqueue children,
        // but because children are in a circular list, we must traverse them carefully.
        
        // Enqueue all children of node
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

/*
 * Print the entire FibHeap in the required format:
 *   - Each tree on a separate line
 *   - Within a tree, do a BFS (level-order).
 *   - "Start with the tree of a smaller degree" => we can sort roots by degree.
 */
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
        // We will do a BFS, but the problem states "Nodes belonging to different trees in the root list are printed on separate lines."
        // Let's just do a level-order for the single tree, printing all nodes in a single line.
        
        // Instead of a BFS that prints multiple lines for levels, we want them all in one line (but still level-order).
        // Let's do a standard BFS but gather the nodes in an array, then print them.
        
        // We can do a queue-based BFS for the single tree:
        Queue q = {0};
        enqueue(&q, roots[i], 0);
        int prevLevel = 0;
        
        // We'll store them in a local list for printing in "level order from left to right".
        // But the problem statement's examples put nodes in the same level with a space,
        // and no explicit line break for deeper levels. 
        // We'll replicate the sample output exactly.
        
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

/* --------------- MAIN: PROCESS COMMANDS --------------- */

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
            // Unknown command, ignore or break
            break;
        }
    }
    
    // After processing all commands, print the final Fibonacci Heap structure
    printFibonacciHeap(heap);
    
    return 0;
}
