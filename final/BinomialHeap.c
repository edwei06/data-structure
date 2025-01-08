#include<stdio.h>
#include<stdlib.h>
#include<limits.h>


typedef struct BinomialNode{
   int key;
   int degree;
   struct BinomialNode *parent;
   struct BinomialNode *child;
   struct BinomialNode *siblings;
} BinomialNode;


BinomialNode* create(int key){
   BinomialNode* node = (BinomialNode*)malloc(sizeof(BinomialNode));
   node->key = key;
   node->degree = 0;
   node->parent = NULL;
   node->child = NULL;
   node->siblings = NULL;
   return node;
}


BinomialNode* mergeTrees(BinomialNode* tree1 , BinomialNode* tree2){
   if(tree1->key > tree2->key){
       BinomialNode *temp = tree1;
       tree1 = tree2;
       tree2 = temp;
   }
   tree2->parent = tree1;
   tree2->siblings = tree1->child;
   tree1->child = tree2;
   tree1->degree++;
   return tree1;
}


BinomialNode* mergeHeaps(BinomialNode *heap1, BinomialNode *heap2){
   if(!heap1) return heap2;
   if(!heap2) return heap1;


   BinomialNode *head = NULL;
   BinomialNode **pos = &head;
   while(heap1 && heap2){
       if(heap1->degree <= heap2->degree){
           *pos = heap1;
           heap1 = heap1->siblings;
       }
       else{
           *pos = heap2;
           heap2 = heap2->siblings;
       }
       pos = &((*pos)->siblings); // Correctly update 'pos'
   }


   if(heap1) *pos = heap1;
   if(heap2) *pos = heap2;


   return head;
}




BinomialNode* unionHeap(BinomialNode* heap1 , BinomialNode* heap2){
   BinomialNode *mergeHeap = mergeHeaps(heap1 , heap2);
   if(!mergeHeap) return NULL;


   BinomialNode *prev = NULL;
   BinomialNode *current = mergeHeap;
   BinomialNode *next = current->siblings;


   while(next){
       if(current->degree != next->degree || (next->siblings && next->siblings->degree == current->degree)){
           prev = current;
           current= next;
       }
       else{
           if(current->key <= next->key){
               current->siblings = next->siblings;
               current = mergeTrees(current , next);
           }
           else{
               if(prev){
                   prev->siblings = next;
               }
               else{
                   mergeHeap = next;
               }
               next = mergeTrees(next , current);
               current = next;
           }
       }
       next = current->siblings;
   }
   return mergeHeap;
}


BinomialNode* insert(BinomialNode* heap , int key){
   BinomialNode *newNode = create(key);
   return unionHeap(heap , newNode);
}


BinomialNode* extractMin(BinomialNode** heap){
   if(!*heap) return NULL;
   BinomialNode* prevMin = NULL;
   BinomialNode* minNode = *heap;
   BinomialNode* prev = NULL;
   BinomialNode* current = *heap;


   // Find the minimum node
   while(current){
       if(current->key < minNode->key){
           minNode = current;
           prevMin = prev;
       }
       prev = current;
       current = current->siblings;
   }


   // Remove minNode from heap
   if(prevMin){
       prevMin->siblings = minNode->siblings;
   }
   else{
       *heap = minNode->siblings;
   }


   // Reverse the order of minNode's children and set their parent to NULL
   BinomialNode* child  = minNode->child;
   BinomialNode* reverseChild = NULL;


   while(child){
       BinomialNode *next= child->siblings;
       child->siblings = reverseChild;
       child->parent = NULL; // Important: Reset parent pointer
       reverseChild = child;
       child = next;
   }


   // Merge the reversed children with the current heap
   *heap = unionHeap(*heap , reverseChild);
   return minNode;
}




void printHeap(BinomialNode *heap) {
   if (!heap) return;
   printf("(key=%d, degree=%d)\n", heap->key, heap->degree);
   if (heap->child) {
       printf("Child of %d:\n", heap->key);
       printHeap(heap->child);
   }
   if (heap->siblings) {
       printf("Sibling of %d:\n", heap->key);
       printHeap(heap->siblings);
   }
}
void levelOrder(BinomialNode* heap){
   if(!heap){
       printf("the heap is empty");
       return;
   }
   BinomialNode** Queue = (BinomialNode**)malloc(100*sizeof(BinomialNode*));
   int front =-1 ,rear = -1;
   int root[10];
   for(int i=0 ; i<10 ; i++){
       root[i] =-1;
   }
   BinomialNode* current = heap;
   //printf("%d " , heap->key);
   while(current){
      
       Queue[++rear] = current;
       //printf("%d\n" , current->key);
       current = current->siblings;
   }
   while(front < rear){
       BinomialNode* node = Queue[++front];
       printf("%d ", node->key);
       BinomialNode* child = node->child;
       if(child){
           BinomialNode* temp = child;
           while(temp){
               Queue[++rear] = temp;
               temp = temp->siblings;
           }
       }
   }
   printf("\n");
   free(Queue);
}


int main() {
   BinomialNode *heap = NULL;


   // 插入測試數據
   heap = insert(heap, 20);
   heap = insert(heap, 10);
   heap = insert(heap, 5);
   heap = insert(heap, 18);
   heap = insert(heap, 6);
   heap = insert(heap, 12);
   heap = insert(heap, 14);
   heap = insert(heap, 4);
   heap = insert(heap, 22);
   printf("Binomial Heap:\n");
   //printHeap(heap);
   levelOrder(heap);


   //提取最小值
   BinomialNode *minNode = extractMin(&heap);
   printf("Extracted Min: %d\n", minNode->key);
   free(minNode);


   printf("Binomial Heap after extraction:\n");
   //printHeap(heap);
   levelOrder(heap);


   return 0;
}