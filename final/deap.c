#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>


int heap[100];
int count = 1;

int find_i(int j){
    for(int i = 2;i <= count;i++){
        int val = i + ( 1 << (int)(log2(i)))/2;
        if(val > count){
            val  /= 2;
        }
        if(val == j && heap[i] > heap[j]){
            return i;
        }
    }
    return -1;
}


void insert_min(int index){ 
    int parent = (index)/2;
    int temp = heap[index];
    int tempIndex = index;
    while(tempIndex > 2 && temp < heap[parent]){
        heap[tempIndex] = heap[parent];
        tempIndex = parent;
        parent /= 2;
    }
    heap[tempIndex] = temp;
}

void insert_max(int index){ 
    int parent = (index)/2;
    int temp = heap[index];
    int tempIndex = index;
    while(tempIndex > 3 && temp > heap[parent]){
        heap[tempIndex] = heap[parent];
        tempIndex = parent;
        parent /= 2;
    }
    heap[tempIndex] = temp;
}

void insert(int data){
    heap[count] = data;
    int level = (int)log2(count) + 1;
    //printf("level = %d\n", level);
    int first = ( 1 << (level-1));
    //printf("first = %d\n", first);
    int bro ;
    if(count < first + (first/2)){
        bro = (int)(count + ( 1 << (int)(log2(count)))/2)/2;  
        //printf("bro = %d\n", bro);
        if(bro > 1){
            if(data > heap[bro]){
                int temp = heap[bro];
                heap[bro] = heap[count];
                heap[count] = temp;
            }
        }
        //printf("insert min\n");
        //printf("adjust = %d ",heap[count]);
        insert_min(count);
    }else{
        bro = find_i(count);
        if(bro > 1){
            int temp = heap[bro];
            heap[bro] = heap[count];
            heap[count] = temp;
        }
        //printf("insert max\n");
        insert_max(count);
    }
}


void delete_min(){
    int child = 4;
    int temp = heap[2];
    int tempindex = 2;
    while(child <= count){
        if(child < count && heap[child] > heap[child+1]){
            child++;
        }
        heap[tempindex] = heap[child];
        tempindex = child;
        child *= 2;
    }
    heap[tempindex] = heap[count];
    count --;
    int level = (int)log2(count) + 1;
    int first = ( 1 << level );
    int bro = (int)tempindex + ( 1 << (int)(log2(tempindex))/2); 
    if(bro > count){
        bro /= 2;
    }
    if(bro > 1){
        if(heap[bro] < heap[tempindex]){
            int temp = heap[bro];
            heap[bro] = heap[tempindex];
            heap[tempindex] = temp;
        }
    }
    insert_min(tempindex);
}

void delete_max(){
    int child = 6;
    int temp = heap[3];
    int tempindex = 3;
    while(child <= count){
        if(child < count && heap[child] < heap[child+1]){
            child++;
        }
        heap[tempindex] = heap[child];
        tempindex = child;
        child *= 2;
    }
    heap[tempindex] = heap[count];
    count --;
    int level = (int)log2(count) + 1;
    int first = ( 1 << level );
    int bro = find_i(tempindex);
    if(bro > 1){
        int temp = heap[bro];
        heap[bro] = heap[tempindex];
        heap[tempindex] = temp;
    }
    insert_max(tempindex);
}

void level_order(){
    for(int i = 2; i <= count;i++){
        printf("%d ", heap[i]);
    }
}

int main(){

    char command[100];
    int key;

    while (scanf("%s", command) != EOF ) {
        if (strcmp(command, "insert") == 0) {
            scanf("%d", &key);
            count ++;
            insert(key);
            //printf("count = %d \n", count);
            //printf("heap[%d] = %d\n", count, heap[count]);
        } else if (strcmp(command, "delete-min") == 0) {
            delete_min();
        } else if(strcmp(command, "delete-max") == 0){
            delete_max();
        } else{
            break;
        }
    }
    level_order();
    return 0;
}