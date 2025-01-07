#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* 
 * Binomial Node structure 
 */
typedef struct BinomialNode {
    int key;                     /* The value stored in the node */
    int degree;                  /* The degree of the node */
    struct BinomialNode* parent; /* Pointer to parent in tree */
    struct BinomialNode* child;  /* Pointer to leftmost child */
    struct BinomialNode* sibling;/* Pointer to right sibling */
} BinomialNode;

/* 
 * The binomial heap is represented by the head of a linked list of root nodes. 
 * Each root node is the start of one binomial tree.
 */
typedef struct BinomialHeap {
    BinomialNode* head; /* Head of the list of root nodes */
} BinomialHeap;

/* Forward declarations of helper functions */
static BinomialNode* createNode(int key);
static BinomialNode* binomialHeapMerge(BinomialNode* h1, BinomialNode* h2);
static void binomialLink(BinomialNode* y, BinomialNode* z);
static BinomialNode* binomialHeapUnion(BinomialNode* h1, BinomialNode* h2);
static BinomialNode* findMinNode(BinomialNode* head);

/* 
 * createHeap - Create an empty binomial heap 
 */
BinomialHeap* createHeap() {
    BinomialHeap* H = (BinomialHeap*)malloc(sizeof(BinomialHeap));
    if (!H) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    H->head = NULL;
    return H;
}

/* 
 * createNode - Allocate and initialize a new BinomialNode 
 */
static BinomialNode* createNode(int key) {
    BinomialNode* node = (BinomialNode*)malloc(sizeof(BinomialNode));
    if (!node) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->degree = 0;
    node->parent = NULL;
    node->child = NULL;
    node->sibling = NULL;
    return node;
}

/* 
 * binomialLink - Make tree rooted at y a subtree of z 
 * (used when merging binomial trees of the same degree).
 */
static void binomialLink(BinomialNode* y, BinomialNode* z) {
    y->parent = z;
    y->sibling = z->child;
    z->child = y;
    z->degree++;
}

/* 
 * binomialHeapMerge - Merge the root lists of two binomial heaps (without fixing degrees).
 * Returns the merged list of roots.
 */
static BinomialNode* binomialHeapMerge(BinomialNode* h1, BinomialNode* h2) {
    /* If one list is empty, return the other. */
    if (!h1) return h2;
    if (!h2) return h1;

    BinomialNode* head = NULL;
    BinomialNode** pos = &head;

    while (h1 && h2) {
        if (h1->degree <= h2->degree) {
            *pos = h1;
            h1 = h1->sibling;
        } else {
            *pos = h2;
            h2 = h2->sibling;
        }
        pos = &((*pos)->sibling);
    }
    *pos = (h1) ? h1 : h2;
    return head;
}

/* 
 * binomialHeapUnion - Union of two binomial heaps by merging root lists 
 * and then fixing up the heap so no two roots have the same degree.
 */
static BinomialNode* binomialHeapUnion(BinomialNode* h1, BinomialNode* h2) {
    BinomialNode* newHead = binomialHeapMerge(h1, h2);
    if (!newHead) return NULL;

    BinomialNode *prev = NULL;
    BinomialNode *curr = newHead;
    BinomialNode *next = curr->sibling;

    while (next) {
        /* 
         * If the current root and next root have different degrees 
         * or curr->degree == next->sibling->degree, move one step ahead.
         */
        if ((curr->degree != next->degree) ||
            (next->sibling && next->sibling->degree == curr->degree)) {
            prev = curr;
            curr = next;
        } else {
            /* If curr->key <= next->key, then next is linked as child of curr. */
            if (curr->key <= next->key) {
                curr->sibling = next->sibling;
                binomialLink(next, curr);
            } else {
                /* Otherwise curr is linked as child of next. */
                if (!prev) {
                    newHead = next;
                } else {
                    prev->sibling = next;
                }
                binomialLink(curr, next);
                curr = next;
            }
        }
        next = curr->sibling;
    }
    return newHead;
}

/* 
 * meldHeaps - Meld (union) two binomial heaps H1 and H2.
 * After meld, H2 is empty, and H1 contains the union.
 */
void meldHeaps(BinomialHeap* H1, BinomialHeap* H2) {
    H1->head = binomialHeapUnion(H1->head, H2->head);
    H2->head = NULL;
}

/* 
 * insert - Insert a new key into the binomial heap.
 */
void insertKey(BinomialHeap* H, int key) {
    BinomialNode* node = createNode(key);

    /* Create a temporary heap for this single node. */
    BinomialHeap* temp = createHeap();
    temp->head = node;

    /* Meld the temporary heap with our main heap. */
    meldHeaps(H, temp);

    /* Clean up the temporary heap structure (but not its node). */
    free(temp);
}

/* 
 * findMinNode - Find the node with the minimum key in the root list.
 */
static BinomialNode* findMinNode(BinomialNode* head) {
    if (!head) return NULL;

    BinomialNode* minNode = head;
    int minValue = head->key;
    BinomialNode* curr = head->sibling;

    while (curr) {
        if (curr->key < minValue) {
            minValue = curr->key;
            minNode = curr;
        }
        curr = curr->sibling;
    }
    return minNode;
}

/* 
 * getMin - Return the minimum key without deleting it.
 * Returns INT_MAX if the heap is empty.
 */
int getMin(BinomialHeap* H) {
    BinomialNode* minNode = findMinNode(H->head);
    if (!minNode) {
        return INT_MAX; /* Indicate the heap is empty */
    }
    return minNode->key;
}

/* 
 * deleteMin - Extract and remove the minimum element from the binomial heap.
 * Returns the key of the deleted node or INT_MAX if the heap is empty.
 */
int deleteMin(BinomialHeap* H) {
    if (!H->head) return INT_MAX; /* Heap is empty */

    /* 1. Find the root with minimum key. */
    BinomialNode* minNode = H->head;
    BinomialNode* prevMinNode = NULL;
    BinomialNode* curr = H->head;
    BinomialNode* prev = NULL;

    int minKey = minNode->key;
    while (curr) {
        if (curr->key < minKey) {
            minKey = curr->key;
            minNode = curr;
            prevMinNode = prev;
        }
        prev = curr;
        curr = curr->sibling;
    }

    /* 2. Remove minNode from the root list. */
    if (prevMinNode) {
        prevMinNode->sibling = minNode->sibling;
    } else {
        H->head = minNode->sibling;
    }

    /* 3. Reverse the order of minNode's children and make them a new heap. */
    BinomialNode* child = minNode->child;
    BinomialNode* prevChild = NULL;
    while (child) {
        BinomialNode* next = child->sibling;
        child->sibling = prevChild;
        child->parent = NULL; /* They are now roots of a new heap */
        prevChild = child;
        child = next;
    }

    /* 4. Union that new heap with the current heap. */
    BinomialNode* newHead = binomialHeapUnion(H->head, prevChild);

    /* 5. Free the minNode. */
    free(minNode);

    /* 6. Update the heap head. */
    H->head = newHead;

    return minKey;
}

/* 
 * Example main for testing 
 */
int main() {
    BinomialHeap* H = createHeap();

    printf("Inserting keys...\n");
    insertKey(H, 10);
    insertKey(H, 3);
    insertKey(H, 7);
    insertKey(H, 2);
    insertKey(H, 8);

    printf("Minimum: %d\n", getMin(H)); // Should be 2

    printf("Deleting minimum...\n");
    int minKey = deleteMin(H);
    printf("Deleted min key: %d\n", minKey);  // Should be 2
    printf("New minimum after delete: %d\n", getMin(H)); // Should be 3

    /* Meld demonstration */
    BinomialHeap* H2 = createHeap();
    insertKey(H2, 1);
    insertKey(H2, 12);

    printf("Melding two heaps...\n");
    meldHeaps(H, H2);
    printf("New minimum after meld: %d\n", getMin(H)); // Should be 1

    /* Clean up: In real use, you'd implement a proper destroy function. */
    // ... (not implemented in this demo)

    return 0;
}
