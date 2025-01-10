#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 1000

// SMMH structure
typedef struct {
    int data[MAX_SIZE];
    int size;
} SMMH;

// Initialize SMMH
void initialize(SMMH* smmh) {
    smmh->size = 0;
    memset(smmh->data, 0, sizeof(smmh->data)); 
}

// Swap two elements
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Helper function to maintain SMMH property after insertion
void bubbleUp(SMMH* smmh, int index) {
    if (index % 2 == 1){
        if (smmh->data[index-1] > smmh->data[index]){
            swap(&smmh->data[index], &smmh->data[index-1]);
            index = index - 1; 
        }
    }
    

    int parent = index / 2;
    if (parent==1) return;

    if (index % 4 < 2) { 
        if (smmh->data[index] > smmh->data[parent + 1]) {
            swap(&smmh->data[index], &smmh->data[parent + 1]);
            bubbleUp(smmh, parent + 1);
        }
        if (smmh->data[index] < smmh->data[parent]) {
            swap(&smmh->data[index], &smmh->data[parent]);
            bubbleUp(smmh, parent);
        }
    } else { 
        if (smmh->data[index] > smmh->data[parent]) {
            swap(&smmh->data[index], &smmh->data[parent]);
            bubbleUp(smmh, parent);
        }
        if (smmh->data[index] < smmh->data[parent - 1]) {
            swap(&smmh->data[index], &smmh->data[parent - 1]);
            bubbleUp(smmh, parent - 1);
        }
    }
}

// Insert a key into SMMH
void insert(SMMH* smmh, int key) {
    if (smmh->size >= MAX_SIZE - 1) return;
    
    smmh->size++;
    smmh->data[smmh->size+1] = key;
    bubbleUp(smmh, smmh->size + 1);
}

void deleteMin(SMMH* smmh) {
    int current = 1;
    int last = smmh->size - 1;
    if(last == 1){
        return;
    }

    if(last != 1) {
        swap(&smmh->data[current], &smmh->data[smmh->size - 1]);
        smmh->size--;

    }
    int done = 0;
    while(!done && (current <= (last-2)/2)) {
        int gp = ((current+1)/2) - 1;
        int gc1 = (4*(gp+1))-1;
        int gc2 = gc1 + 2;

        if(gc2 < last) {
            if(smmh->data[gc1] > smmh->data[gc2]) {
                gc1 = gc2;
        }
        }
         
         if(smmh->data[gc1] < smmh->data[current]){
           swap(&smmh->data[current],&smmh->data[gc1]);
           current = gc1;  
         }
         else{
            done = 1;
         }
        
    }

    if((smmh->data[current] > smmh->data[current+1]) && (current+1) < smmh->size) {
            swap(&smmh->data[current],&smmh->data[current+1]); 
    }
        
}

void deleteMax(SMMH* smmh) {
    int current = 2;
    int last = smmh->size - 1;
    if(last == 2){
        return;
    }
    
    if(last != 2) {
        swap(&smmh->data[current], &smmh->data[smmh->size - 1]);
        smmh->size--;

    }
    int done = 0;
    while(!done && (current <= (last-1)/2)) {
        int gp = ((current)/2) - 1;
        int gc1 = (4*(gp+1));
        int gc2 = gc1 + 2;

        if(gc2 < last) {
        if(smmh->data[gc1] < smmh->data[gc2]) {
                gc1 = gc2;
        }
        }
        
         
         if(smmh->data[gc1] > smmh->data[current]){
           swap(&smmh->data[current],&smmh->data[gc1]);
           current = gc1;  
         }
         else{
            done = 1;
         }
        
    }

    if(smmh->data[current] < smmh->data[current-1]) {
            swap(&smmh->data[current],&smmh->data[current-1]); 
    }
}
void printLevelOrder(SMMH* smmh) {
    if (smmh->size == 0 ) {
        printf("\n");
        return;
    }
    for (int i = 2; i <= smmh->size+1; i++) {
        printf("%d", smmh->data[i]);
        if (i < smmh->size + 1) {
            printf(" ");
        }
    }
    printf("\n");
}

int main() {
    SMMH smmh;
    initialize(&smmh);
    
    char command[20];
    int key;

    while (1) {
        scanf("%s", command);
        if (strcmp(command, "exit") == 0) {
            printLevelOrder(&smmh);
            break;
        } else if (strcmp(command, "insert") == 0) {
            scanf("%d", &key);
            insert(&smmh, key);
        } else if (strcmp(command, "delete-min") == 0) {
            deleteMin(&smmh);
        } else if (strcmp(command, "delete-max") == 0) {
            deleteMax(&smmh);
        }  
    } return 0;
}