// implement hash table using shift folding hash function to store intger keys complete function shiftfoldinghash and insertkey
// the hash function is defined as follows: partition each input key into groups of 3 starting from msd and add all 3-digit partitions together eg. 12345 => h(12345) = 123+45
// hash(key) = (sum of partitions) % n
// quadratic probing (without alternating sign) this is used to resolve overflow when a bucket is full the probing sequence is h(k,i) = (h(k) + i^2)

// implement hash table using shift folding hash function to store intger keys complete function shiftfoldinghash
// the hash function is defined as follows: partition each input key into groups of 3 starting from msd and add all 3-digit partitions together eg. 12345 => h(12345) = 123+45=168
// F74126288_final y22Bsv
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY -1    // Marker for an empty slot

int *hashTable;     // Dynamically allocated hash table
int n;              // Number of buckets

//Write a function to insert the values and shiftFoldingHash
int shiftFoldingHash(int key) {
    // Your answer
}

void insertKey(int key) {
    // Your answer
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