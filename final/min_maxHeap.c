//hp arr[1000] start from 1
//0<key

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>


void insert( int* mh, int size, int key);
bool isminlevel(int x);
void varifyMin(int* mh, int i, int x);
void varifyMax(int* mh, int i, int x);
void deleteMin(int* mh, int size);
//void deleteMax(int* mh, int size);
int minChildGrandChild(int* mh, int i, int size);

bool isminlevel(int x){
    int y = log2(x);
    return y % 2 == 0;
}
void insert( int* mh, int size, int key){
    if( size == 10000){
        printf("The heap is full\n");
        return;
    }

    mh[size] = key;
    int p = size / 2; //parent

    //mh is empty
    if(p == 0){
        mh[1] = key;
        return;
    }else{
        if(isminlevel(p)){
            //parent is min level
            if(key < mh[p]){
                int tmp = mh[p];
                mh[p] = key;
                mh[size] = tmp;
                //key is in min level
                varifyMin(mh, p, key);
            }else{
                varifyMax(mh, size, key);
            }

        }else{
            if(key > mh[p]){
                int tmp = mh[p];
                mh[p] = key;
                mh[size] = tmp;
                varifyMax(mh, p, key);
            }else{
                varifyMin(mh, size, key);
            }
        }
    }   
}


void varifyMin( int* mh, int i, int x){//i為index, x為key
    int gp = i/4;
    while( gp > 0 ){
        if(mh[i] < mh [gp]){
            mh[i] = mh[gp];
            i = gp;
            gp = i/4;
        }else{
            break;
        }
    } 
    mh[i] = x;
}

void varifyMax( int* mh, int i, int x){
    int gp = i/4;
    while( gp > 0 ){
        if(mh[i] > mh[gp]){
            mh[i] = mh[gp];
            i = gp;
            gp = i/4;
        }else{
            break;
        }
    }
    mh[i] = x;
}

void deleteMin( int* mh, int size){
    if(size == 0){
        printf("The heap is empty\n");
        return;
    }

    int x = mh[size];
    mh[size] = 0;
    size--;
    int i = 1;
    int j = size/2; //last parent
    bool notDone = true;
    
    while( i<=j && notDone == true ){ //i has child
        int k = minChildGrandChild(mh, i, size);
        if( x <= mh[k]){
            //x is the smallest (root)
            notDone = false;
        }else{
            
            mh[i] = mh[k];
            if( k <= 2*i+1 ){
                // k is child
                mh[k] = x;
                notDone = false;
            }else{
                //if k is grandChild
                int p = k/2;
                if( x > mh[p]){
                    mh[k] = mh[p];
                    mh[p] = x;
                    i = k;
                    x= mh[k];
                }else{
                    mh[k] = x;
                    i = k;
                }
            }
                
        }

    }
}

int minChildGrandChild( int* mh, int i, int size){
    int min;
    if(isminlevel(i) && (4*i+3 <= size)){
        //min level is grandChild
        int index = 4*i;
        min = index;
        for( int j = 1 ; j<4 ; j++){
            if(mh[index+j] == 0){
                break;  
            }
            if(mh[index+j] < mh[min]){
                min = index+j;
            }
        }
        printf("minCG is grand child\n");
    }else{
        //min level is child
        int index = 2*i;
        min = index;
        if(mh[index+1] != 0 && mh[index+1] < mh[min]){
            min = index+1;
        }
        printf("minCG is child\n");

    }

    printf("minCG = %d\n", min);
    return min;
}

int main(){

    int mh[1000] = {0};
    int mh_size = 0;

    char cmd[30];
    while( scanf("%s", cmd) != EOF){
        if(strcmp(cmd, "insert") == 0){
            int key;
            scanf("%d", &key);
            mh_size++;
            insert(mh, mh_size, key);
        }
        else if(strcmp(cmd, "delete-min") == 0){
            deleteMin(mh, mh_size);
            mh_size--;
           
        }
        else if(strcmp(cmd, "exit") == 0){
            for(int i = 1; i <= mh_size; i++){
                printf("%d ", mh[i]);
            }
            printf("\n");
            break;  
        }
    }
}

