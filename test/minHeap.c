#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEAP_SIZE 1000

typedef struct {
    int data[MAX_HEAP_SIZE];
    int size;
} MinHeap;

// Function to swap two integers
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Initialize the heap
void initHeap(MinHeap *heap) {
    heap->size = 0;
}

// Heapify up to maintain the Min-Heap property
void heapifyUp(MinHeap *heap, int index) {
    if (index && heap->data[index] < heap->data[(index - 1) / 2]) {
        swap(&heap->data[index], &heap->data[(index - 1) / 2]);
        heapifyUp(heap, (index - 1) / 2);
    }
}

// Heapify down to maintain the Min-Heap property
void heapifyDown(MinHeap *heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && heap->data[left] < heap->data[smallest])
        smallest = left;
    if (right < heap->size && heap->data[right] < heap->data[smallest])
        smallest = right;
    if (smallest != index) {
        swap(&heap->data[index], &heap->data[smallest]);
        heapifyDown(heap, smallest);
    }
}

// Insert a new element into the heap
void insertElement(MinHeap *heap, int key) {
    if (heap->size == MAX_HEAP_SIZE) {
        printf("Heap is full\n");
        return;
    }
    heap->data[heap->size] = key;
    heapifyUp(heap, heap->size);
    heap->size++;
}

// Delete a specific element from the heap
void deleteElement(MinHeap *heap, int key) {
    int i;
    // Find the index of the key to be deleted
    for (i = 0; i < heap->size; i++) {
        if (heap->data[i] == key)
            break;
    }
    if (i == heap->size) {
        // Element not found
        printf("Element %d not found in the heap.\n", key);
        return;
    }
    // Replace with the last element
    heap->data[i] = heap->data[heap->size - 1];
    heap->size--;
    // Heapify to maintain heap property
    heapifyDown(heap, i);
    heapifyUp(heap, i);
}

// Update a specific element in the heap
void updateElement(MinHeap *heap, int oldKey, int newKey) {
    int i;
    // Find the index of the oldKey to be updated
    for (i = 0; i < heap->size; i++) {
        if (heap->data[i] == oldKey)
            break;
    }
    if (i == heap->size) {
        // Element not found
        printf("Element %d not found in the heap.\n", oldKey);
        return;
    }
    // Update the value
    heap->data[i] = newKey;
    // Restore the heap property
    heapifyDown(heap, i);
    heapifyUp(heap, i);
}

// Get the minimum element (root) of the heap
void getMin(MinHeap *heap) {
    if (heap->size == 0) {
        printf("Heap is empty.\n");
        return;
    }
    printf("%d\n", heap->data[0]);
}

// Delete the minimum element (root) of the heap
void delMin(MinHeap *heap) {
    if (heap->size == 0) {
        printf("Heap is empty.\n");
        return;
    }
    int min = heap->data[0];
    printf("Deleted min element: %d\n", min);
    // Replace root with the last element
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    // Heapify down to maintain heap property
    heapifyDown(heap, 0);
}

// Print the heap in level-order
void printHeap(MinHeap *heap) {
    if (heap->size == 0) {
        printf("Heap is empty.\n");
        return;
    }
    for (int i = 0; i < heap->size; i++) {
        printf("%d ", heap->data[i]);
    }
    printf("\n");
}

int main() {
    MinHeap heap;
    initHeap(&heap);

    char command[20];
    int value1, value2;

    printf("Enter commands (insert x, delete x, update x y, getMin, delMin, exit):\n");

    while (1) {
        if (scanf("%s", command) != 1) {
            break;
        }

        if (strcmp(command, "insert") == 0) {
            if (scanf("%d", &value1) != 1) {
                printf("Invalid input for insert\n");
                continue;
            }
            insertElement(&heap, value1);
        }
        else if (strcmp(command, "delete") == 0) {
            if (scanf("%d", &value1) != 1) {
                printf("Invalid input for delete\n");
                continue;
            }
            deleteElement(&heap, value1);
        }
        else if (strcmp(command, "update") == 0) {
            if (scanf("%d %d", &value1, &value2) != 2) {
                printf("Invalid input for update\n");
                continue;
            }
            updateElement(&heap, value1, value2);
        }
        else if (strcmp(command, "getMin") == 0) {
            getMin(&heap);
        }
        else if (strcmp(command, "delMin") == 0) {
            delMin(&heap);
        }
        else if (strcmp(command, "exit") == 0) {
            break;
        }
        else {
            printf("Unknown command: %s\n", command);
            // Clear the remaining input for this line
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }

    printf("Final heap (level-order):\n");
    printHeap(&heap);
    return 0;
}
