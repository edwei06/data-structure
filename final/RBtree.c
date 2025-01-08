#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//去年作業沒有delete所以有關delete的扣是gpt寫的然後還沒測過，可以就先參考一下
//那個新增函數就是gpt寫的，應該都是有關delete的，啊insert和search是沒問題的
typedef enum { RED, BLACK } Color;

typedef struct Node {
    int key;
    Color color;
    struct Node* left;
    struct Node* right;
    struct Node* parent;
} Node;

Node* createNode(int key);
void leftRotate(Node** root, Node* x);
void rightRotate(Node** root, Node* y);
Node* search(Node* root, int x);
Node* insert(Node* root, int data);
void insertFixup(Node** root, Node* z);
void freeTree(Node* root);

/* ============================= 新增以下函式 ============================= */

/* 
 * 用來在樹中替換 u 與 v，讓 u 的父節點指向 v。
 * 若 u 的 parent 為 NULL，代表 u 為根節點，則直接把 *root 設為 v。
 */
void rbTransplant(Node** root, Node* u, Node* v) {
    if (u->parent == NULL) {
        *root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v != NULL) {
        v->parent = u->parent;
    }
}

/* 取得某顆子樹裡最小 key 的節點（一直往 left 走到底） */
Node* treeMinimum(Node* x) {
    while (x->left != NULL) {
        x = x->left;
    }
    return x;
}

/* 刪除後為了維持 Red-Black Tree 性質，需要對 x 進行調整 */
void deleteFixup(Node** root, Node* x) {
    while (x != *root && (x == NULL || x->color == BLACK)) {
        if (x == x->parent->left) {
            Node* w = x->parent->right;
            // Case 1: x 的兄弟節點 w 是紅色
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                leftRotate(root, x->parent);
                w = x->parent->right;
            }
            // Case 2: w 的兩個子節點都為黑色
            if ((w->left == NULL || w->left->color == BLACK) &&
                (w->right == NULL || w->right->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                // Case 3: w 的左子節點是紅色、右子節點是黑色
                if (w->left != NULL && w->left->color == RED &&
                    (w->right == NULL || w->right->color == BLACK)) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotate(root, w);
                    w = x->parent->right;
                }
                // Case 4: w 的右子節點是紅色
                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->right != NULL) {
                    w->right->color = BLACK;
                }
                leftRotate(root, x->parent);
                x = *root;
            }
        } else {
            // 與上方對稱的情況
            Node* w = x->parent->left;
            // Case 1
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(root, x->parent);
                w = x->parent->left;
            }
            // Case 2
            if ((w->right == NULL || w->right->color == BLACK) &&
                (w->left == NULL || w->left->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                // Case 3
                if (w->right != NULL && w->right->color == RED &&
                    (w->left == NULL || w->left->color == BLACK)) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotate(root, w);
                    w = x->parent->left;
                }
                // Case 4
                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->left != NULL) {
                    w->left->color = BLACK;
                }
                rightRotate(root, x->parent);
                x = *root;
            }
        }
    }
    if (x != NULL) {
        x->color = BLACK;
    }
}

/* 在紅黑樹中刪除 key 對應的節點 */
Node* deleteNode(Node* root, int key) {
    // 先找要刪除的節點 z
    Node* z = search(root, key);
    if (z == NULL) {
        // 若找不到就直接回傳
        return root;
    }

    Node* y = z;                  // y 為實際被刪除的節點（可能是 z 或它的後繼）
    Color yOriginalColor = y->color;
    Node* x = NULL;               // x 為用來替代 y 的節點

    // 情況 1：z 沒有左子樹，直接用右子樹取代 z
    if (z->left == NULL) {
        x = z->right;
        rbTransplant(&root, z, z->right);
    }
    // 情況 2：z 沒有右子樹，直接用左子樹取代 z
    else if (z->right == NULL) {
        x = z->left;
        rbTransplant(&root, z, z->left);
    }
    // 情況 3：z 同時有左右子樹，需找其後繼（右子樹中最小的）
    else {
        y = treeMinimum(z->right);
        yOriginalColor = y->color;
        x = y->right; // y 一定沒有左子樹（因為它是最小的）

        // 若 y 的父親就是 z，則需要特別處理 x 的 parent
        if (y->parent == z) {
            if (x != NULL) {
                x->parent = y;
            }
        } else {
            // 先用 y->right 替換 y
            rbTransplant(&root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        // 最後用 y 替換 z
        rbTransplant(&root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    // 釋放 z
    free(z);

    // 若被真正移除的 y 的顏色是黑色，則需要 fixup
    if (yOriginalColor == BLACK) {
        deleteFixup(&root, x);
    }
    return root;
}

/* ============================= 以上為新增函式 ============================= */

/* 建立新節點的函式（原程式有） */
Node* createNode(int key) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->key = key;
    newNode->color = RED; // 新插入的節點預設為紅色
    newNode->left = newNode->right = newNode->parent = NULL;
    return newNode;
}

/* 左旋轉（原程式有） */
void leftRotate(Node** root, Node* x) {
    Node* y = x->right;
    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

/* 右旋轉（原程式有） */
void rightRotate(Node** root, Node* y) {
    Node* x = y->left;
    y->left = x->right;
    if (x->right != NULL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NULL) {
        *root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

/* 插入修正函式（原程式有） */
void insertFixup(Node** root, Node* z) {
    while (z->parent != NULL && z->parent->color == RED) {
        Node* parent = z->parent;
        Node* grandparent = parent->parent;
        if (parent == grandparent->left) {
            Node* uncle = grandparent->right;
            if (uncle != NULL && uncle->color == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == parent->right) {
                    z = parent;
                    leftRotate(root, z);
                    parent = z->parent;
                    grandparent = parent->parent;
                }
                parent->color = BLACK;
                grandparent->color = RED;
                rightRotate(root, grandparent);
            }
        } else {
            Node* uncle = grandparent->left;
            if (uncle != NULL && uncle->color == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == parent->left) {
                    z = parent;
                    rightRotate(root, z);
                    parent = z->parent;
                    grandparent = parent->parent;
                }
                parent->color = BLACK;
                grandparent->color = RED;
                leftRotate(root, grandparent);
            }
        }
    }
    (*root)->color = BLACK;
}

/* 插入 (原程式有) */
Node* insert(Node* root, int data) {
    Node* z = (Node*)malloc(sizeof(Node));
    z->key = data;
    z->left = NULL;
    z->right = NULL;
    z->parent = NULL;
    z->color = RED;

    Node* y = NULL;
    Node* x = root;
    while (x != NULL) {
        y = x;
        if (data < x->key)
            x = x->left;
        else if (data > x->key)
            x = x->right;
        else {
            // 若值已存在，直接回傳
            free(z);
            return root;
        }
    }
    z->parent = y;
    if (y == NULL) {
        root = z;
    } else if (data < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }
    insertFixup(&root, z);
    return root;
}

/* 搜尋 (原程式有) */
Node* search(Node* root, int x) {
    while (root != NULL && root->key != x) {
        if (x < root->key)
            root = root->left;
        else
            root = root->right;
    }
    return root;
}

/* 釋放整棵樹 (原程式有) */
void freeTree(Node* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

/* 主程式 (原程式) */
int main() {
    Node* root = NULL;
    int value;
    char operation[20];

    while (1) {
        scanf("%s", operation);
        if (strcmp(operation, "insert") == 0) {
            scanf("%d", &value);
            root = insert(root, value);
        } else if (strcmp(operation, "search") == 0) {
            scanf("%d", &value);
            Node* result = search(root, value);
            if (result != NULL) {
                printf("%s\n", (result->color == RED) ? "red" : "black");
            } else {
                printf("Not found\n");
            }
        } else if (strcmp(operation, "delete") == 0) {
            scanf("%d", &value);
            // 呼叫我們新增的函式
            root = deleteNode(root, value);
            // 這裡你可以自行加上例如印出 inorder traversal 的程式碼
            // printf("In-order traversal after deletion...\n");
        } else if (strcmp(operation, "quit") == 0) {
            break;
        }
    }

    freeTree(root);
    return 0;
}