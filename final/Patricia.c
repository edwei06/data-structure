#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BITS 32

typedef struct Node {
   int bitIndex;
   unsigned data;
   struct Node *left;
   struct Node *right;
} Node;

int getBit(unsigned key, int idx) {
   return (key & (1 << (MAX_BITS - idx))) != 0;
}

Node* createNode(int bitIndex, unsigned data) {
   Node* node = (Node*)malloc(sizeof(Node));
   node->bitIndex = bitIndex;
   node->data = data;
   node->left = node->right = NULL;
   return node;
}

Node* search(Node* t, unsigned k) {
   if (t == NULL) return NULL;

   Node *currentNode, *nextNode;
   nextNode = t->left;
   currentNode = t;

   while (nextNode->bitIndex > currentNode->bitIndex) {
       currentNode = nextNode;
       nextNode = getBit(k, nextNode->bitIndex) ? nextNode->right : nextNode->left;
   }

   return nextNode;
}

Node* insert(Node* t, unsigned key) {
   Node *current = NULL, *parent, *lastNode, *newNode;
   int i;

   if (t == NULL) {
       t = createNode(0, key);
       t->left = t;
       return t;
   }

   lastNode = search(t, key);
   if (key == lastNode->data) {
       printf("Key already Present\n");
       return t;
   }

   // Find the first bit position (i) where key differs from lastNode->data
   for (i = 1; getBit(key, i) == getBit(lastNode->data, i); i++);

   current = t->left;
   parent = t;

   // 1. current node tests a bit position higher than its parent
   // 2. current node tests a bit position lower than our differing bit (i)
   while (current->bitIndex > parent->bitIndex && current->bitIndex < i) {
       parent = current;
       current = getBit(key, current->bitIndex) ? current->right : current->left;
   }

   newNode = createNode(i, key);

   //adjust newNode's pointers
   if(getBit(key, i)){
       newNode->left=current;
       newNode->right=newNode;
   }else{
       newNode->right=current;
       newNode->left=newNode;
   }

   //adjust parent
   if (current == parent->left) {
       parent->left = newNode;
   } else {
       parent->right = newNode;
   }

   return t;
}
Node* findParent(Node* root, int key){
   if(!root) return NULL;
   Node* next=root->left, *cur=root;
   while(next->bitIndex > cur->bitIndex){
       cur=next;
       next = getBit(key, next->bitIndex)? next->right:next->left;
   }

   return cur;
}
Node* delete(Node* root, int key){
   Node* node = search(root, key);
   if(node==NULL || node->data!=key) return root;
   Node* parent=findParent(root, key);

   if(node->left!=node && node->right!=node){
       //no self pointer, copy parent's val to current
       Node* gParent = findParent(root, parent->data);
       if(getBit(parent->data,gParent->bitIndex))
           gParent->right = node;

       else gParent->left = node;

       node->data=parent->data;
   }else{
       Node* next = (node->right==node)? node->left:node->right;
       if(getBit(key, parent->bitIndex)){
           parent->right=next;
       }else{
           parent->left=next;
       }
   }
   free(parent);

   return root;
}
void testSearch(Node *root, unsigned key) {
   Node *node = search(root, key);
   if (node->data==key) {
       printf("Key %u found.\n", key);
   } else {
       printf("Key %u not found.\n", key);
   }
}
int main() {
   Node* root = NULL;
   root = insert(root, 5);
   root = insert(root, 15);
   root = insert(root, 20);
   root = insert(root, 7);
   root = insert(root, 100);

   testSearch(root,5);
   testSearch(root,15);
   testSearch(root,7);
   testSearch(root,12);

   root=delete(root, 5);
   root=delete(root, 15);
   root=delete(root, 12);
   root=delete(root, 7);
   printf("\n");
   testSearch(root,12);
   testSearch(root,5);
   testSearch(root,15);
   testSearch(root,7);
  
   testSearch(root,100);
   return 0;
}


