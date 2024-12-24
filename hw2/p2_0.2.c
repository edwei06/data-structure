#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Fibonacci Heap Node structure
typedef struct FibNode {
    int key;
    int degree;
    struct FibNode* parent;
    struct FibNode* child;
    struct FibNode* left;
    struct FibNode* right;
    int mark;
} FibNode;

// Fibonacci Heap structure
typedef struct FibHeap {
    FibNode* min;
    int n;
} FibHeap;

// Function to create a new Fibonacci Heap node
FibNode* create_node(int key) {
    FibNode* node = (FibNode*)malloc(sizeof(FibNode));
    node->key = key;
    node->degree = 0;
    node->parent = NULL;
    node->child = NULL;
    node->left = node;
    node->right = node;
    node->mark = 0;
    return node;
}

// Initialize a new Fibonacci Heap
FibHeap* create_heap() {
    FibHeap* heap = (FibHeap*)malloc(sizeof(FibHeap));
    heap->min = NULL;
    heap->n = 0;
    return heap;
}

// Insert a node into the heap
void fib_heap_insert(FibHeap* heap, FibNode* node) {
    if (heap->min == NULL) {
        heap->min = node;
    } else {
        // Insert into root list
        node->left = heap->min;
        node->right = heap->min->right;
        heap->min->right->left = node;
        heap->min->right = node;
        if (node->key < heap->min->key) {
            heap->min = node;
        }
    }
    heap->n++;
}

// Find the node with the minimum key
FibNode* fib_heap_min(FibHeap* heap) {
    return heap->min;
}

// Link two trees of the same degree
void fib_heap_link(FibHeap* heap, FibNode* y, FibNode* x) {
    // Remove y from root list
    y->left->right = y->right;
    y->right->left = y->left;
    // Make y a child of x
    y->parent = x;
    if (x->child == NULL) {
        x->child = y;
        y->left = y->right = y;
    } else {
        y->left = x->child;
        y->right = x->child->right;
        x->child->right->left = y;
        x->child->right = y;
    }
    x->degree++;
    y->mark = 0;
}

// Consolidate the heap after extract-min or delete
void fib_heap_consolidate(FibHeap* heap) {
    int array_size = 45; // Enough for n <= 100
    FibNode** A = (FibNode**)malloc(sizeof(FibNode*) * array_size);
    for (int i = 0; i < array_size; i++) A[i] = NULL;

    // List of root nodes
    FibNode* start = heap->min;
    if (start == NULL) {
        free(A);
        return;
    }
    FibNode* w = start;
    do {
        FibNode* x = w;
        int d = x->degree;
        while (A[d] != NULL) {
            FibNode* y = A[d];
            if (x->key > y->key) {
                FibNode* temp = x;
                x = y;
                y = temp;
            }
            fib_heap_link(heap, y, x);
            A[d] = NULL;
            d++;
        }
        A[d] = x;
        w = w->right;
    } while (w != start);

    // Reconstruct the root list
    heap->min = NULL;
    for (int i = 0; i < array_size; i++) {
        if (A[i] != NULL) {
            if (heap->min == NULL) {
                heap->min = A[i];
                A[i]->left = A[i]->right = A[i];
            } else {
                // Insert into root list
                A[i]->left = heap->min;
                A[i]->right = heap->min->right;
                heap->min->right->left = A[i];
                heap->min->right = A[i];
                if (A[i]->key < heap->min->key) {
                    heap->min = A[i];
                }
            }
        }
    }
    free(A);
}

// Extract the minimum node from the heap
FibNode* fib_heap_extract_min(FibHeap* heap) {
    FibNode* z = heap->min;
    if (z != NULL) {
        // Add children to root list
        if (z->child != NULL) {
            FibNode* child = z->child;
            do {
                FibNode* next = child->right;
                // Add to root list
                child->left->right = child->right;
                child->right->left = child->left;
                child->parent = NULL;
                child->left = heap->min;
                child->right = heap->min->right;
                heap->min->right->left = child;
                heap->min->right = child;
                if (child->key < heap->min->key) {
                    heap->min = child;
                }
                child = next;
            } while (child != z->child);
        }
        // Remove z from root list
        z->left->right = z->right;
        z->right->left = z->left;
        if (z == z->right) {
            heap->min = NULL;
        } else {
            heap->min = z->right;
            fib_heap_consolidate(heap);
        }
        heap->n--;
    }
    return z;
}

// Merge two Fibonacci Heaps
FibHeap* fib_heap_union(FibHeap* h1, FibHeap* h2) {
    if (h1->min == NULL) return h2;
    if (h2->min == NULL) return h1;
    // Merge the root lists
    FibNode* h1_min_right = h1->min->right;
    h1->min->right = h2->min->right;
    h2->min->right->left = h1->min;
    h2->min->right = h1_min_right;
    h1_min_right->left = h2->min;
    // Update min pointer
    if (h2->min->key < h1->min->key) {
        h1->min = h2->min;
    }
    h1->n += h2->n;
    free(h2);
    return h1;
}

// Search for a node with a given key
FibNode* fib_heap_search(FibHeap* heap, FibNode* node, int key) {
    if (node == NULL) return NULL;
    FibNode* found = NULL;
    FibNode* start = node;
    do {
        if (node->key == key) {
            return node;
        }
        if ((found = fib_heap_search(heap, node->child, key)) != NULL) {
            return found;
        }
        node = node->right;
    } while (node != start);
    return NULL;
}

// Cut the link between node and its parent, and add node to root list
void fib_heap_cut(FibHeap* heap, FibNode* x, FibNode* y) {
    // Remove x from child list of y
    if (x->right == x) {
        y->child = NULL;
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x) {
            y->child = x->right;
        }
    }
    y->degree--;
    // Add x to root list
    x->left = heap->min;
    x->right = heap->min->right;
    heap->min->right->left = x;
    heap->min->right = x;
    x->parent = NULL;
    x->mark = 0;
}

// Perform cascading cuts
void fib_heap_cascading_cut(FibHeap* heap, FibNode* y) {
    FibNode* z = y->parent;
    if (z != NULL) {
        if (y->mark == 0) {
            y->mark = 1;
        } else {
            fib_heap_cut(heap, y, z);
            fib_heap_cascading_cut(heap, z);
        }
    }
}

// Decrease the key of a node
void fib_heap_decrease_key(FibHeap* heap, FibNode* x, int delta) {
    x->key -= delta;
    FibNode* y = x->parent;
    if (y != NULL && x->key < y->key) {
        fib_heap_cut(heap, x, y);
        fib_heap_cascading_cut(heap, y);
    }
    if (x->key < heap->min->key) {
        heap->min = x;
    }
}

// Delete a node from the heap
void fib_heap_delete(FibHeap* heap, FibNode* x) {
    fib_heap_decrease_key(heap, x, x->key + 1); // Assuming keys are positive
    fib_heap_extract_min(heap);
}

// Function to perform level-order traversal and collect nodes by level
typedef struct QueueNode {
    FibNode* node;
    int level;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
} Queue;

void enqueue(Queue* q, FibNode* node, int level) {
    QueueNode* temp = (QueueNode*)malloc(sizeof(QueueNode));
    temp->node = node;
    temp->level = level;
    temp->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

QueueNode* dequeue(Queue* q) {
    if (q->front == NULL) return NULL;
    QueueNode* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    return temp;
}

void free_queue(Queue* q) {
    while (q->front != NULL) {
        QueueNode* temp = dequeue(q);
        free(temp);
    }
}

// Function to print the heap in level-order traversal
void print_heap(FibHeap* heap) {
    if (heap->min == NULL) return;

    // First, collect all root nodes
    // To sort roots by degree, we need to traverse and store them
    // Since n <= 100, we can use an array
    int max_degree = 45; // Enough for n <= 100
    FibNode** roots = (FibNode**)malloc(sizeof(FibNode*) * max_degree);
    int root_count = 0;

    FibNode* start = heap->min;
    FibNode* w = start;
    do {
        roots[root_count++] = w;
        w = w->right;
    } while (w != start);

    // Sort roots by degree
    for (int i = 0; i < root_count - 1; i++) {
        for (int j = i + 1; j < root_count; j++) {
            if (roots[i]->degree > roots[j]->degree) {
                FibNode* temp = roots[i];
                roots[i] = roots[j];
                roots[j] = temp;
            } else if (roots[i]->degree == roots[j]->degree) {
                if (roots[i]->key > roots[j]->key) {
                    FibNode* temp = roots[i];
                    roots[i] = roots[j];
                    roots[j] = temp;
                }
            }
        }
    }

    // For each root, perform level-order traversal
    for (int i = 0; i < root_count; i++) {
        FibNode* root = roots[i];
        // Perform BFS
        Queue q;
        q.front = q.rear = NULL;
        enqueue(&q, root, 0);
        int current_level = 0;
        int first = 1;
        while (q.front != NULL) {
            QueueNode* qnode = dequeue(&q);
            if (qnode->level > current_level) {
                current_level = qnode->level;
                printf("\n");
                first = 1;
            }
            if (!first) printf(" ");
            printf("%d", qnode->node->key);
            first = 0;
            // Enqueue children
            // To sort children in ascending order, collect them first
            // and sort before enqueuing
            int child_count = qnode->node->degree;
            if (child_count > 0) {
                FibNode** children = (FibNode**)malloc(sizeof(FibNode*) * child_count);
                FibNode* child = qnode->node->child;
                int idx = 0;
                do {
                    children[idx++] = child;
                    child = child->right;
                } while (child != qnode->node->child && idx < child_count);
                // Sort children by key
                for (int m = 0; m < child_count - 1; m++) {
                    for (int n = m + 1; n < child_count; n++) {
                        if (children[m]->key > children[n]->key) {
                            FibNode* temp = children[m];
                            children[m] = children[n];
                            children[n] = temp;
                        }
                    }
                }
                // Enqueue sorted children
                for (int m = 0; m < child_count; m++) {
                    enqueue(&q, children[m], qnode->level + 1);
                }
                free(children);
            }
            free(qnode);
        }
        free_queue(&q);
        printf("\n");
    }
    free(roots);
}

int main() {
    FibHeap* heap = create_heap();
    char command[50];
    while (fgets(command, sizeof(command), stdin)) {
        // Remove trailing newline
        command[strcspn(command, "\n")] = 0;
        if (strncmp(command, "insert ", 7) == 0) {
            int key;
            sscanf(command + 7, "%d", &key);
            FibNode* node = create_node(key);
            fib_heap_insert(heap, node);
        } else if (strncmp(command, "delete ", 7) == 0) {
            int key;
            sscanf(command + 7, "%d", &key);
            FibNode* node = fib_heap_search(heap, heap->min, key);
            if (node != NULL) {
                fib_heap_delete(heap, node);
            }
        } else if (strncmp(command, "decrease ", 9) == 0) {
            int key, delta;
            sscanf(command + 9, "%d %d", &key, &delta);
            FibNode* node = fib_heap_search(heap, heap->min, key);
            if (node != NULL) {
                fib_heap_decrease_key(heap, node, delta);
            }
        } else if (strcmp(command, "extract-min") == 0) {
            FibNode* min = fib_heap_extract_min(heap);
            // You can print or handle the extracted min if needed
            // For this problem, we don't need to
            free(min);
        } else if (strcmp(command, "exit") == 0) {
            break;
        }
    }
    // Print the final heap structure
    print_heap(heap);
    // Cleanup: Ideally, you'd free all allocated memory here
    return 0;
}
