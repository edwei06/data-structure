#include <stdio.h>
#include <stdlib.h>

// Definition of a BST node
typedef struct Node {
    int key;
    struct Node* left;
    struct Node* right;
} Node;
// Function to create a new BST node
Node* createNode(int key) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    newNode->key = key;
    newNode->left = newNode->right = NULL;
    return newNode;
}
// Function to insert a key into the BST
Node* insert(Node* root, int key) {
    // If the tree is empty, return a new node
    if (root == NULL) {
        return createNode(key);
    }

    // Otherwise, recur down the tree
    if (key < root->key) {
        root->left = insert(root->left, key);
    }
    else if (key > root->key) {
        root->right = insert(root->right, key);
    }
    // If the key is already present, do not insert duplicates
    return root;
}
// Function for in-order traversal of the BST
void inorderTraversal(Node* root) {
    if (root != NULL) {
        inorderTraversal(root->left);
        printf("%d ", root->key);
        inorderTraversal(root->right);
    }
}
// Function to search for a key in the BST
Node* search(Node* root, int key) {
    // Base Cases: root is null or key is present at root
    if (root == NULL || root->key == key)
        return root;

    // Key is greater than root's key
    if (key > root->key)
        return search(root->right, key);

    // Key is smaller than root's key
    return search(root->left, key);
}
// Function to find the minimum value node in a BST
Node* findMin(Node* root) {
    while (root->left != NULL)
        root = root->left;
    return root;
}

// Function to delete a key from the BST
Node* deleteNode(Node* root, int key) {
    if (root == NULL) return root;

    // If the key to be deleted is smaller than the root's key,
    // then it lies in the left subtree
    if (key < root->key)
        root->left = deleteNode(root->left, key);

    // If the key to be deleted is greater than the root's key,
    // then it lies in the right subtree
    else if (key > root->key)
        root->right = deleteNode(root->right, key);

    // If key is same as root's key, then this is the node to be deleted
    else {
        // Node with only one child or no child
        if (root->left == NULL) {
            Node* temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL) {
            Node* temp = root->left;
            free(root);
            return temp;
        }

        // Node with two children:
        // Get the inorder successor (smallest in the right subtree)
        Node* temp = findMin(root->right);

        // Copy the inorder successor's content to this node
        root->key = temp->key;

        // Delete the inorder successor
        root->right = deleteNode(root->right, temp->key);
    }
    return root;
}
int main() {
    Node* root = NULL;
    int choice, key;

    while (1) {
        printf("\nBinary Search Tree Operations:\n");
        printf("1. Insert\n");
        printf("2. In-order Traversal\n");
        printf("3. Search\n");
        printf("4. Delete\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            break;
        }

        switch (choice) {
            case 1:
                printf("Enter key to insert: ");
                if (scanf("%d", &key) != 1) {
                    printf("Invalid input!\n");
                    break;
                }
                root = insert(root, key);
                printf("Key %d inserted.\n", key);
                break;

            case 2:
                printf("In-order Traversal: ");
                inorderTraversal(root);
                printf("\n");
                break;

            case 3:
                printf("Enter key to search: ");
                if (scanf("%d", &key) != 1) {
                    printf("Invalid input!\n");
                    break;
                }
                {
                    Node* result = search(root, key);
                    if (result != NULL)
                        printf("Key %d found in the BST.\n", key);
                    else
                        printf("Key %d not found in the BST.\n", key);
                }
                break;

            case 4:
                printf("Enter key to delete: ");
                if (scanf("%d", &key) != 1) {
                    printf("Invalid input!\n");
                    break;
                }
                root = deleteNode(root, key);
                printf("Key %d deleted (if it existed).\n", key);
                break;

            case 5:
                printf("Exiting...\n");
                exit(EXIT_SUCCESS);

            default:
                printf("Invalid choice! Please enter a number between 1 and 5.\n");
        }
    }

    return 0;
}
