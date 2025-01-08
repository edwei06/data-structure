#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//delete有時候好像怪怪的但我盡力了
//這個版本的insert和search應該也是沒問題的

typedef struct Node {
    int *keys;
    struct Node **childs;
    int key_count;
    int child_count;
} Node;

typedef struct BTree {
    int order;
    Node *root;
} BTree;

Node *create_node(int order) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->keys = (int *)malloc((2 * order - 1) * sizeof(int));
    node->childs = (Node **)malloc((2 * order) * sizeof(Node *));
    node->key_count = 0;
    node->child_count = 0;
    return node;
}

BTree *create_btree(int order) {
    BTree *btree = (BTree *)malloc(sizeof(BTree));
    btree->order = order;
    btree->root = create_node(order);
    return btree;
}

int get_child_index(Node *node, int x) {
    int i = 0;
    while (i < node->key_count && node->keys[i] < x) {
        i++;
    }
    return i;
}

void split_node(BTree *btree, Node *parent, int i) {
    int order = btree->order;
    Node *child = parent->childs[i];
    Node *right = create_node(order);
    
    right->key_count = order - 1;
    for (int j = 0; j < order - 1; j++) {
        right->keys[j] = child->keys[j + order];
    }
    
    if (child->child_count > 0) {
        right->child_count = order;
        for (int j = 0; j < order; j++) {
            right->childs[j] = child->childs[j + order];
        }
    }

    child->key_count = order - 1;
    parent->childs[parent->child_count] = NULL; // Ensure proper shifting

    for (int j = parent->key_count; j > i; j--) {
        parent->keys[j] = parent->keys[j - 1];
        parent->childs[j + 1] = parent->childs[j];
    }

    parent->keys[i] = child->keys[order - 1];
    parent->childs[i + 1] = right;
    parent->key_count++;
    parent->child_count++;
}

void insert_non_full(BTree *btree, Node *node, int x) {
    int i = node->key_count - 1;

    if (node->child_count == 0) {
        while (i >= 0 && node->keys[i] > x) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = x;
        node->key_count++;
    } else {
        int idx = get_child_index(node, x);
        if (node->childs[idx]->key_count == 2 * btree->order - 1) {
            split_node(btree, node, idx);
            if (x > node->keys[idx]) {
                idx++;
            }
        }
        insert_non_full(btree, node->childs[idx], x);
    }
}

void insert(BTree *btree, int x) {
    if (btree->root->key_count == 2 * btree->order - 1) {
        Node *new_root = create_node(btree->order);
        new_root->childs[0] = btree->root;
        new_root->child_count = 1;
        split_node(btree, new_root, 0);
        btree->root = new_root;
    }
    insert_non_full(btree, btree->root, x);
}

void rotate(BTree *btree, Node *node, int i) {
    int order = btree->order;

    // 如果左邊的子節點有多餘的元素
    if (node->childs[i]->key_count > order - 1) {
        // 將父節點的鍵插入右子節點的最左邊
        for (int j = node->childs[i + 1]->key_count; j > 0; j--) {
            node->childs[i + 1]->keys[j] = node->childs[i + 1]->keys[j - 1];
        }
        node->childs[i + 1]->keys[0] = node->keys[i];
        node->childs[i + 1]->key_count++;

        // 如果左子節點有子節點，也需要移動子節點
        if (node->childs[i]->child_count > 0) {
            for (int j = node->childs[i + 1]->child_count; j > 0; j--) {
                node->childs[i + 1]->childs[j] = node->childs[i + 1]->childs[j - 1];
            }
            node->childs[i + 1]->childs[0] = node->childs[i]->childs[node->childs[i]->child_count - 1];
            node->childs[i]->child_count--;
        }

        // 更新父節點的鍵
        node->keys[i] = node->childs[i]->keys[node->childs[i]->key_count - 1];
        node->childs[i]->key_count--;
    } else { // 如果右邊的子節點有多餘的元素
        // 將父節點的鍵插入左子節點的最右邊
        node->childs[i]->keys[node->childs[i]->key_count] = node->keys[i];
        node->childs[i]->key_count++;

        // 如果右子節點有子節點，也需要移動子節點
        if (node->childs[i + 1]->child_count > 0) {
            node->childs[i]->childs[node->childs[i]->child_count] = node->childs[i + 1]->childs[0];
            for (int j = 0; j < node->childs[i + 1]->child_count - 1; j++) {
                node->childs[i + 1]->childs[j] = node->childs[i + 1]->childs[j + 1];
            }
            node->childs[i + 1]->child_count--;
        }

        // 更新父節點的鍵
        node->keys[i] = node->childs[i + 1]->keys[0];
        for (int j = 0; j < node->childs[i + 1]->key_count - 1; j++) {
            node->childs[i + 1]->keys[j] = node->childs[i + 1]->keys[j + 1];
        }
        node->childs[i + 1]->key_count--;
    }
}

Node *combine(BTree *btree, Node *node, int i) {
    int order = btree->order;

    // 當根節點只剩一個鍵時，合併兩個子節點並提升樹層
    if (node == btree->root && node->key_count == 1) {
        Node *tmp = btree->root;
        Node *new_root = create_node(order);

        // 合併左子節點的鍵
        for (int k = 0; k < tmp->childs[0]->key_count; k++) {
            new_root->keys[new_root->key_count++] = tmp->childs[0]->keys[k];
        }

        // 合併父節點的鍵
        new_root->keys[new_root->key_count++] = tmp->keys[0];

        // 合併右子節點的鍵
        for (int k = 0; k < tmp->childs[1]->key_count; k++) {
            new_root->keys[new_root->key_count++] = tmp->childs[1]->keys[k];
        }

        // 合併子節點的子節點
        if (tmp->childs[0]->child_count > 0) {
            for (int c = 0; c < tmp->childs[0]->child_count; c++) {
                new_root->childs[new_root->child_count++] = tmp->childs[0]->childs[c];
            }
        }
        if (tmp->childs[1]->child_count > 0) {
            for (int c = 0; c < tmp->childs[1]->child_count; c++) {
                new_root->childs[new_root->child_count++] = tmp->childs[1]->childs[c];
            }
        }

        // 更新根節點
        btree->root = new_root;
        free(tmp);
        return new_root;
    } else {
        // 當節點有多於兩個子節點時，合併相鄰的子節點
        Node *new_node = create_node(order);

        // 合併左子節點的鍵
        for (int k = 0; k < node->childs[i]->key_count; k++) {
            new_node->keys[new_node->key_count++] = node->childs[i]->keys[k];
        }

        // 合併父節點的鍵
        new_node->keys[new_node->key_count++] = node->keys[i];

        // 合併右子節點的鍵
        for (int k = 0; k < node->childs[i + 1]->key_count; k++) {
            new_node->keys[new_node->key_count++] = node->childs[i + 1]->keys[k];
        }

        // 合併左子節點和右子節點的子節點
        if (node->childs[i]->child_count > 0) {
            for (int c = 0; c < node->childs[i]->child_count; c++) {
                new_node->childs[new_node->child_count++] = node->childs[i]->childs[c];
            }
        }
        if (node->childs[i + 1]->child_count > 0) {
            for (int c = 0; c < node->childs[i + 1]->child_count; c++) {
                new_node->childs[new_node->child_count++] = node->childs[i + 1]->childs[c];
            }
        }

        // 移除父節點中的鍵和子節點
        for (int k = i; k < node->key_count - 1; k++) {
            node->keys[k] = node->keys[k + 1];
        }
        node->key_count--;

        for (int c = i + 1; c < node->child_count - 1; c++) {
            node->childs[c] = node->childs[c + 1];
        }
        node->child_count--;

        // 將合併後的新節點插入到父節點的子節點中
        node->childs[i] = new_node;

        return node;
    }
}

Node *search(Node *node, int x, int *index) {
    int i = 0;
    while (i < node->key_count && node->keys[i] < x) {
        i++;
    }
    if (i < node->key_count && node->keys[i] == x) {
        *index = i;
        return node;
    }
    if (node->child_count == 0) {
        return NULL;
    }
    return search(node->childs[i], x, index);
}

// Helper function to find the minimum key in the right child
int find_right_child_min(Node *node) {
    while (node->child_count > 0) {
        node = node->childs[0];
    }
    return node->keys[0];
}

// Helper function to delete a key from the last level
bool delete_last_level(BTree *btree, Node *node, int x) {
    int order = btree->order;
    bool finish = false;
    int i = 0;

    while (i < node->key_count && node->keys[i] < x) {
        i++;
    }

    if (i < node->key_count && node->keys[i] == x) {
        for (int j = i; j < node->key_count - 1; j++) {
            node->keys[j] = node->keys[j + 1];
        }
        node->key_count--;
        finish = true;
        return finish;
    }
    if (!finish && node->childs[i]->key_count == order - 1) {
        if (i == 0) {
            if (node->childs[i + 1]->key_count > order - 1) {
                rotate(btree, node, i);
            } else {
                node = combine(btree, node, i);
                return delete_last_level(btree, node, x);
            }
        } else if (i == node->child_count - 1) {
            if (node->childs[i - 1]->key_count > order - 1) {
                rotate(btree, node, i - 1);
            } else {
                node = combine(btree, node, i - 1);
                return delete_last_level(btree, node, x);
            }
        } else {
            if (node->childs[i + 1]->key_count > order - 1) {
                rotate(btree, node, i);
            } else if (node->childs[i - 1]->key_count > order - 1) {
                rotate(btree, node, i - 1);
            } else {
                node = combine(btree, node, i);
                return delete_last_level(btree, node, x);
            }
        }
    }

    return delete_last_level(btree, node->childs[i], x);
}


void delete_key(BTree *btree, Node *node, int x) {
    int index;
    Node *target_node = search(node, x, &index);

    if (target_node == NULL) {
        printf("Key not found.\n");
        return;
    }

    if (target_node->child_count == 0) {
        delete_last_level(btree, btree->root, x);
    } else {
        int y = find_right_child_min(target_node->childs[index + 1]);
        delete_last_level(btree, btree->root, y);
        target_node->keys[index] = y;
    }
}

void print_btree(Node *node, int level) {
    if (node == NULL) return;

    for (int i = 0; i < level; i++) printf("  ");
    for (int i = 0; i < node->key_count; i++) {
        printf("%d ", node->keys[i]);
    }
    printf("\n");

    for (int i = 0; i < node->child_count; i++) {
        print_btree(node->childs[i], level + 1);
    }
}

int main() {
    int order = 2;
    BTree *btree = create_btree(order);

    int insert_data[] = {36, 83, 85, 93, 12, 98, 2, 6, 57, 79};
    int delete_data[] = {2, 93, 6, 12, 83, 36, 85, 79, 57, 98};

    printf("Insertion Process:\n");
    for (int i = 0; i < 10; i++) {
        insert(btree, insert_data[i]);
        print_btree(btree->root, 1);
        printf("\n");
    }

    printf("Deletion Process:\n");
    for (int i = 0; i < 10; i++) {
        delete_key(btree, btree->root, delete_data[i]);
        print_btree(btree->root, 1);
        printf("\n");
    }

    return 0;
}
