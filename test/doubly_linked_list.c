#include <stdio.h>
#include <stdlib.h>

// Define the structure for a doubly linked list node
typedef struct Node {
    int data;
    struct Node* next;
    struct Node* prev;
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
    newNode->prev = NULL; // Initialize prev pointer to NULL
    return newNode;       // Return the new node
}
// Function to insert a node at the beginning of the list
void insertAtBeginning(Node** head_ref, int data) {
    Node* newNode = createNode(data); // Create a new node
    newNode->next = *head_ref;        // Point new node's next to current head
    newNode->prev = NULL;             // New node is the first node, so prev is NULL

    if (*head_ref != NULL) {          // If the list is not empty
        (*head_ref)->prev = newNode;  // Update the previous head's prev to new node
    }

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
    newNode->prev = temp;              // Link new node back to the last node
}
// Function to insert a node after a given node
void insertAfter(Node* prev_node, int data) {
    if (prev_node == NULL) { // Check if the given previous node is NULL
        printf("The given previous node cannot be NULL.\n");
        return;
    }

    Node* newNode = createNode(data);    // Create a new node
    newNode->next = prev_node->next;     // Link new node to the next node
    newNode->prev = prev_node;           // Link new node back to previous node

    if (prev_node->next != NULL) {       // If not inserting after the last node
        prev_node->next->prev = newNode; // Update the next node's prev to new node
    }

    prev_node->next = newNode;           // Link previous node to the new node
}
// Function to delete the first occurrence of a key in the list
void deleteNode(Node** head_ref, int key) {
    Node* temp = *head_ref; // Start from the head

    // Traverse the list to find the node to be deleted
    while (temp != NULL && temp->data != key) {
        temp = temp->next;
    }

    // If the key was not present in the list
    if (temp == NULL) {
        printf("Key %d not found in the list.\n", key);
        return;
    }

    // If node to be deleted is the head node
    if (temp->prev == NULL) {
        *head_ref = temp->next; // Update head
    } else {
        temp->prev->next = temp->next; // Link previous node to next node
    }

    // If node to be deleted is not the last node
    if (temp->next != NULL) {
        temp->next->prev = temp->prev; // Link next node back to previous node
    }

    free(temp); // Free memory
}
// Function to print the linked list from head to tail
void printListForward(Node* head) {
    Node* temp = head;
    printf("Forward Traversal: ");
    while (temp != NULL) {                   // Traverse until the end of the list
        printf("%d <-> ", temp->data);       // Print current node's data
        temp = temp->next;                    // Move to the next node
    }
    printf("NULL\n");                         // Indicate the end of the list
}
// Function to print the linked list from tail to head
void printListBackward(Node* head) {
    Node* temp = head;

    // Traverse to the last node
    while (temp != NULL && temp->next != NULL) {
        temp = temp->next;
    }

    printf("Backward Traversal: ");
    // Traverse backward using prev pointers
    while (temp != NULL) {
        printf("%d <-> ", temp->data);       // Print current node's data
        temp = temp->prev;                    // Move to the previous node
    }
    printf("NULL\n");                         // Indicate the start of the list
}
int main() {
    Node* head = NULL; // Initialize an empty list

    // Insert nodes
    insertAtEnd(&head, 10);           // List: 10 <-> NULL
    insertAtBeginning(&head, 20);     // List: 20 <-> 10 <-> NULL
    insertAtEnd(&head, 30);           // List: 20 <-> 10 <-> 30 <-> NULL
    insertAfter(head->next, 25);      // List: 20 <-> 10 <-> 25 <-> 30 <-> NULL

    printf("Initial Doubly Linked List:\n");
    printListForward(head);    // Output: 20 <-> 10 <-> 25 <-> 30 <-> NULL
    printListBackward(head);   // Output: 30 <-> 25 <-> 10 <-> 20 <-> NULL

    // Delete a node
    deleteNode(&head, 20); // Delete node with data 20
    printf("\nAfter deleting 20:\n");
    printListForward(head);    // Output: 10 <-> 25 <-> 30 <-> NULL
    printListBackward(head);   // Output: 30 <-> 25 <-> 10 <-> NULL

    // Attempt to delete a non-existent node
    deleteNode(&head, 100); // Output: Key 100 not found in the list.

    // Insert more nodes
    insertAtBeginning(&head, 40); // List: 40 <-> 10 <-> 25 <-> 30 <-> NULL
    insertAtEnd(&head, 50);       // List: 40 <-> 10 <-> 25 <-> 30 <-> 50 <-> NULL
    printf("\nAfter inserting 40 at beginning and 50 at end:\n");
    printListForward(head);    // Output: 40 <-> 10 <-> 25 <-> 30 <-> 50 <-> NULL
    printListBackward(head);   // Output: 50 <-> 30 <-> 25 <-> 10 <-> 40 <-> NULL

    // Free remaining nodes to prevent memory leaks
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }

    return 0;
}
