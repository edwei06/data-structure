#include <stdio.h>
#include <stdlib.h>

// Define the structure for a linked list node
typedef struct Node {
    int data;
    struct Node* next;
} Node;
// Function to create a new node with given data
Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) { // Check if memory allocation was successful
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data; // Assign data to the node
    newNode->next = NULL; // Initialize next pointer to NULL
    return newNode;       // Return the new node
}
// Function to insert a node at the beginning of the list
void insertAtBeginning(Node** head_ref, int data) {
    Node* newNode = createNode(data); // Create a new node
    newNode->next = *head_ref;        // Point new node to current head
    *head_ref = newNode;              // Update head to new node
}
// Function to insert a node at the end of the list
void insertAtEnd(Node** head_ref, int data) {
    Node* newNode = createNode(data); // Create a new node
    if (*head_ref == NULL) {          // If the list is empty
        *head_ref = newNode;           // New node becomes the head
        return;
    }
    Node* temp = *head_ref;
    while (temp->next != NULL) {       // Traverse to the last node
        temp = temp->next;
    }
    temp->next = newNode;              // Link the last node to the new node
}
// Function to insert a node after a given node
void insertAfter(Node* prev_node, int data) {
    if (prev_node == NULL) { // Check if the given previous node is NULL
        printf("The given previous node cannot be NULL.\n");
        return;
    }
    Node* newNode = createNode(data);    // Create a new node
    newNode->next = prev_node->next;     // Link new node to the next node
    prev_node->next = newNode;           // Link previous node to the new node
}
// Function to delete the first occurrence of a key in the list
void deleteNode(Node** head_ref, int key) {
    Node* temp = *head_ref; // Start from the head
    Node* prev = NULL;

    // If the head node itself holds the key
    if (temp != NULL && temp->data == key) {
        *head_ref = temp->next; // Change head
        free(temp);             // Free old head
        return;
    }

    // Search for the key
    while (temp != NULL && temp->data != key) {
        prev = temp;
        temp = temp->next;
    }

    // If the key was not present in the list
    if (temp == NULL) {
        printf("Key %d not found in the list.\n", key);
        return;
    }

    // Unlink the node from the list
    prev->next = temp->next;
    free(temp); // Free memory
}
// Function to print the linked list
void printList(Node* head) {
    Node* temp = head;
    while (temp != NULL) {          // Traverse until the end of the list
        printf("%d -> ", temp->data); // Print current node's data
        temp = temp->next;           // Move to the next node
    }
    printf("NULL\n");                // Indicate the end of the list
}
int main() {
    Node* head = NULL; // Initialize an empty list

    // Insert nodes
    insertAtEnd(&head, 10);           // List: 10 -> NULL
    insertAtBeginning(&head, 20);     // List: 20 -> 10 -> NULL
    insertAtEnd(&head, 30);           // List: 20 -> 10 -> 30 -> NULL
    insertAfter(head->next, 25);      // List: 20 -> 10 -> 25 -> 30 -> NULL

    printf("Linked List: ");
    printList(head); // Output: 20 -> 10 -> 25 -> 30 -> NULL

    // Delete a node
    deleteNode(&head, 20); // Delete node with data 20
    printf("After deleting 20: ");
    printList(head); // Output: 10 -> 25 -> 30 -> NULL

    // Attempt to delete a non-existent node
    deleteNode(&head, 100); // Output: Key 100 not found in the list.

    // Free remaining nodes to prevent memory leaks
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }

    return 0;
}
