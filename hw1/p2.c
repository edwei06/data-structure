#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Define maximum heap size
#define MAX_HEAP_SIZE 100000

// Heap structure
typedef struct {
    int arr[MAX_HEAP_SIZE];
    int size;
} MinHeap;

// Function to initialize the heap
void initHeap(MinHeap *heap) {
    heap->size = 0;
}

// Swap two elements in the heap
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Percolate up to maintain heap property
void percolateUp(MinHeap *heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap->arr[index] < heap->arr[parent]) {
            swap(&heap->arr[index], &heap->arr[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

// Percolate down to maintain heap property
void percolateDown(MinHeap *heap, int index) {
    while (1) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heap->size && heap->arr[left] < heap->arr[smallest]) {
            smallest = left;
        }

        if (right < heap->size && heap->arr[right] < heap->arr[smallest]) {
            smallest = right;
        }

        if (smallest != index) {
            swap(&heap->arr[index], &heap->arr[smallest]);
            index = smallest;
        } else {
            break;
        }
    }
}

// Insert an element into the heap
void insert(MinHeap *heap, int key) {
    if (heap->size >= MAX_HEAP_SIZE) {
        // Heap is full, cannot insert
        return;
    }
    heap->arr[heap->size] = key;
    percolateUp(heap, heap->size);
    heap->size++;
}

// Find the index of a given key in the heap
int find(MinHeap *heap, int key) {
    for (int i = 0; i < heap->size; i++) {
        if (heap->arr[i] == key) {
            return i;
        }
    }
    return -1; // Not found
}

// Delete an element from the heap
void deleteKey(MinHeap *heap, int key) {
    int index = find(heap, key);
    if (index == -1) {
        // Key not found, do nothing
        return;
    }

    // Replace with the last element
    heap->arr[index] = heap->arr[heap->size - 1];
    heap->size--;

    if (index < heap->size) {
        // Try percolating up and down to restore heap property
        percolateUp(heap, index);
        percolateDown(heap, index);
    }
}

int main() {
    MinHeap heap;
    initHeap(&heap);

    char command[20];
    int value;

    // Read until EOF
    while (scanf("%s", command) != EOF) {
        if (strcmp(command, "insert") == 0) {
            if (scanf("%d", &value) != 1) {
                // Invalid input, exit
                break;
            }
            insert(&heap, value);
        } else if (strcmp(command, "delete") == 0) {
            if (scanf("%d", &value) != 1) {
                // Invalid input, exit
                break;
            }
            deleteKey(&heap, value);
        }
        // Ignore invalid commands
    }

    // Print the heap in level-order
    for (int i = 0; i < heap.size; i++) {
        printf("%d ", heap.arr[i]);
    }

    return 0;
}