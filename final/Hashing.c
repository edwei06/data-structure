// pass submission
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Define maximum limits based on problem constraints
#define MAX_BUCKETS 100
#define MAX_SLOTS 20
#define MAX_COMMAND_LENGTH 100

// Enumeration to represent the status of a slot
typedef enum {
    EMPTY,
    OCCUPIED,
    DELETED
} SlotStatus;

// Structure to represent each slot in a bucket
typedef struct {
    int key;
    SlotStatus status;
} Slot;

// Structure to represent a bucket containing multiple slots
typedef struct {
    Slot slots[MAX_SLOTS];
} Bucket;

// Global hash table
Bucket hash_table[MAX_BUCKETS];
int n; // Number of buckets
int m; // Number of slots per bucket

// Function to initialize the hash table
void initialize_hash_table() {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            hash_table[i].slots[j].status = EMPTY;
            hash_table[i].slots[j].key = -1; // Initialize with an invalid key
        }
    }
}

// Function to insert a key into the hash table
void insert_key(int key) {
    int hash = key % n;
    for(int probe = 0; probe < n; probe++) {
        int bucket_index = (hash + probe) % n;
        // Find the first available slot in the current bucket
        for(int slot = 0; slot < m; slot++) {
            if(hash_table[bucket_index].slots[slot].status == EMPTY ||
               hash_table[bucket_index].slots[slot].status == DELETED) {
                hash_table[bucket_index].slots[slot].key = key;
                hash_table[bucket_index].slots[slot].status = OCCUPIED;
                return;
            }
        }
        // If current bucket is full, continue to the next bucket
    }
    // If all buckets are full, do nothing as per problem statement
}

// Function to search for a key in the hash table
// Returns 1 if found and sets bucket_index and slot_index, otherwise returns 0
int search_key(int key, int *bucket_index, int *slot_index) {
    int hash = key % n;
    for(int probe = 0; probe < n; probe++) {
        int current_bucket = (hash + probe) % n;
        for(int slot = 0; slot < m; slot++) {
            if(hash_table[current_bucket].slots[slot].status == OCCUPIED) {
                if(hash_table[current_bucket].slots[slot].key == key) {
                    *bucket_index = current_bucket;
                    *slot_index = slot;
                    return 1;
                }
            }
            else if(hash_table[current_bucket].slots[slot].status == EMPTY) {
                // If an EMPTY slot is found, the key does not exist
                return 0;
            }
            // If DELETED, continue searching
        }
    }
    return 0;
}

// Function to delete a key from the hash table
void delete_key(int key) {
    int bucket_index, slot_index;
    if(search_key(key, &bucket_index, &slot_index)) {
        hash_table[bucket_index].slots[slot_index].status = DELETED;
        // Optionally, reset the key value
        hash_table[bucket_index].slots[slot_index].key = -1;
    }
    // As per problem statement, no need to handle non-existent keys
}

int main() {
    char line[MAX_COMMAND_LENGTH];

    // Read the first line: "bucket <n>"
    if(fgets(line, sizeof(line), stdin) != NULL) {
        if(sscanf(line, "bucket %d", &n) != 1) {
            fprintf(stderr, "Invalid input for number of buckets.\n");
            return 1;
        }
    }

    // Read the second line: "slot <m>"
    if(fgets(line, sizeof(line), stdin) != NULL) {
        if(sscanf(line, "slot %d", &m) != 1) {
            fprintf(stderr, "Invalid input for number of slots.\n");
            return 1;
        }
    }

    // Initialize the hash table
    initialize_hash_table();

    // Process commands
    while(fgets(line, sizeof(line), stdin) != NULL) {
        // Remove trailing newline character
        line[strcspn(line, "\n")] = '\0';

        // Check for "exit" command
        if(strcmp(line, "exit") == 0) {
            break;
        }

        // Parse the command
        char command[10];
        int key;
        if(sscanf(line, "%s %d", command, &key) < 1) {
            // Invalid command, skip
            continue;
        }

        if(strcmp(command, "insert") == 0) {
            insert_key(key);
        }
        else if(strcmp(command, "search") == 0) {
            int bucket_index, slot_index;
            if(search_key(key, &bucket_index, &slot_index)) {
                printf("%d %d\n", bucket_index, slot_index);
            }
            // As per problem statement, the key always exists
        }
        else if(strcmp(command, "delete") == 0) {
            delete_key(key);
        }
        // Ignore any other commands
    }

    return 0;
}
