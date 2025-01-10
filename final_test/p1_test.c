#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY -1    // Marker for an empty slot

int *hashTable;     // Dynamically allocated hash table
int n;              // Number of buckets

int shiftFoldingHash(int key) {
    int sum = 0;
    int i = 0;
    int key2 = key;
    while (key2 > 0) {
        i++; 
        key2 /= 10;
    }
    int p = i%3;
    if (p == 1){
        sum += key%10;
        key /= 10;
        while (key>0){
            sum+=key%1000;
            key /= 1000;
        }
    } else if (p == 2){
        sum += key%100;
        key /= 100;
        while (key>0){
            sum += key%1000;
            key /= 1000;
            // printf("sum =%d\n", sum);
        }
    } else {
        while (key>0){
            sum += key%1000;
            key /= 1000;
            // printf("sum =%d\n", sum);
        }
    }
    return sum % n;
}

void insertKey(int key) {
    int hashIndex = shiftFoldingHash(key);
    int i = 0;
    int newIndex;

    while (1) {
        newIndex = (hashIndex + i * i) % n;
        if (hashTable[newIndex] == EMPTY) {
            hashTable[newIndex] = key;
            return;
        }
        i++;
    }
}

void searchKey(int key) {
    int h = shiftFoldingHash(key);
    int i = 0;
    while (i < n) {
        int pos = (h + i*i) % n;
        // Found the key
        if (hashTable[pos] == key) {
            printf("%d\n", pos);
            return;
        }
        i++;
    }
}
void deleteKey(int key) {
    int h = shiftFoldingHash(key);
    int i = 0;
    while (i < n) {
        int pos = (h + i*i) % n;
        if (hashTable[pos] == key) {
            hashTable[pos] = EMPTY;
            return;
        }
        i++;
    }
}

int main() {

    scanf("bucket %d", &n);

    // Allocate the hash table
    hashTable = (int*)malloc(sizeof(int)*n);
    for (int i = 0; i < n; i++) {
        hashTable[i] = EMPTY;  // Initialize to empty
    }

    // Read commands
    while (1) {
        char cmd[16];
        if (scanf("%s", cmd) != 1) break;

        if (strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "insert") == 0) {
            int key;
            scanf("%d", &key);
            insertKey(key);
        } else if (strcmp(cmd, "delete") == 0) {
            int key;
            scanf("%d", &key);
            deleteKey(key);
        } else if (strcmp(cmd, "search") == 0) {
            int key;
            scanf("%d", &key);
            searchKey(key);
        }
    }

    free(hashTable);
    return 0;
}