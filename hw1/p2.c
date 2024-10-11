#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the maximum size of the heap
#define MAX_HEAP_SIZE 1000

// Function to swap two integers
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Function to initialize the heap
void initHeap(int heap[], int *size) {
    *size = 0;
}

// Function to find the index of an element in the heap
int findElement(int heap[], int size, int x) {
    for(int i = 0; i < size; i++) {
        if(heap[i] == x)
            return i;
    }
    return -1; // Element not found
}

// Function to heapify up from a given index
void heapify_up(int heap[], int index) {
    while(index > 0) {
        int parent = (index - 1) / 2;
        if(heap[index] < heap[parent]) {
            swap(&heap[index], &heap[parent]);
            index = parent;
        }
        else {
            break;
        }
    }
}

// Function to heapify down from a given index
void heapify_down(int heap[], int size, int index) {
    while(2 * index + 1 < size) { // While there is at least a left child
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if(left < size && heap[left] < heap[smallest]) {
            smallest = left;
        }
        if(right < size && heap[right] < heap[smallest]) {
            smallest = right;
        }
        if(smallest != index) {
            swap(&heap[index], &heap[smallest]);
            index = smallest;
        }
        else {
            break;
        }
    }
}

// Function to insert an element into the heap
void insertHeap(int heap[], int *size, int x) {
    if(*size >= MAX_HEAP_SIZE) {
        printf("Heap Overflow\n");
        exit(1);
    }
    heap[*size] = x;
    heapify_up(heap, *size);
    (*size)++;
}

// Function to delete an element from the heap
void deleteHeap(int heap[], int *size, int x) {
    int index = findElement(heap, *size, x);
    if(index == -1) {
        // Element not found; according to problem statement, assume valid operations
        // Optionally, print a message or handle the error
        return;
    }

    // Replace the element with the last element
    heap[index] = heap[*size - 1];
    (*size)--;

    // Heapify down from the replaced position
    heapify_down(heap, *size, index);

    // Additionally, heapify up in case the replaced element is smaller than its parent
    heapify_up(heap, index);
}

// Function to print the heap in level-order
void printHeap(int heap[], int size) {
    if(size == 0) {
        printf("\n");
        return;
    }
    printf("%d", heap[0]);
    for(int i = 1; i < size; i++) {
        printf(" %d", heap[i]);
    }
    printf("\n");
}

int main() {
    int heap[MAX_HEAP_SIZE];
    int size;
    initHeap(heap, &size);

    char command[10];
    int number;
    printf("the program will stop once you type exit or it's EOF\n");
    // Read commands until "exit" is entered
    while(1) {
        if(scanf("%s", command) != 1) {
            break; // EOF reached
        }

        if(strcmp(command, "exit") == 0) {
            break; // Exit command received
        }

        if(strcmp(command, "insert") == 0) {
            if(scanf("%d", &number) != 1) {
                printf("Invalid input for insert command.\n");
                exit(1);
            }
            insertHeap(heap, &size, number);
        }
        else if(strcmp(command, "delete") == 0) {
            if(scanf("%d", &number) != 1) {
                printf("Invalid input for delete command.\n");
                exit(1);
            }
            deleteHeap(heap, &size, number);
        }
        else {
            printf("Invalid command: %s\n", command);
            exit(1);
        }
    }

    // Print the final heap in level-order
    printHeap(heap, size);

    return 0;
}
