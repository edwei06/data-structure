#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 定義堆疊的最大大小
#define MAX 1000

// 堆疊結構，用於存放字串
typedef struct {
    char *arr[MAX];
    int top;
} Stack;

// 初始化堆疊
void initStack(Stack *s){
    s->top = -1;
}

// 將字串推入堆疊
void push(Stack *s, const char *str){
    if(s->top >= MAX -1){
        printf("堆疊溢出\n");
        exit(1);
    }
    s->arr[++(s->top)] = strdup(str);
    if (s->arr[s->top] == NULL) {
        printf("記憶體分配錯誤\n");
        exit(1);
    }
}

// 從堆疊中彈出字串
char* pop(Stack *s){
    if(s->top < 0){
        printf("堆疊下溢\n");
        exit(1);
    }
    return s->arr[(s->top)--];
}

// 判斷字符是否為運算符
int isOperator(char c){
    return c == '+' || c == '-' || c == '*' || c == '/';
}

// 將後綴轉換為中置，支援多位數字和多字符操作數
void postfixToInfixMultiDigit(const char *postfix, char *infix){
    Stack stack;
    initStack(&stack);
    
    // 創建一個可修改的副本
    char *expr = strdup(postfix);
    if(expr == NULL){
        printf("記憶體分配錯誤\n");
        exit(1);
    }
    
    // 使用 strtok 分割字串，假設以空格作為分隔符
    char *token = strtok(expr, " ");
    
    while(token != NULL){
        if(!isOperator(token[0]) || strlen(token) > 1){
            // 操作數：將其推入堆疊
            push(&stack, token);
        }
        else{
            // 運算符：彈出兩個操作數，組合成中置表達式，並推回堆疊
            if(stack.top < 1){
                printf("無效的後綴表達式\n");
                free(expr);
                exit(1);
            }
            char *op2 = pop(&stack);
            char *op1 = pop(&stack);
            
            // 創建新的中置表達式 "(op1 operator op2)"
            int newExprLen = strlen(op1) + 1 + strlen(op2) + 3; // 包括括號
            char *exprNew = (char *)malloc(newExprLen * sizeof(char));
            if(!exprNew){
                printf("記憶體分配錯誤\n");
                free(expr);
                exit(1);
            }
            sprintf(exprNew, "(%s%c%s)", op1, token[0], op2);
            
            // 將新的表達式推入堆疊
            push(&stack, exprNew);
            
            // 釋放已使用的記憶體
            free(op1);
            free(op2);
            free(exprNew); // 因為 push 已經複製了字串，所以可以釋放臨時字串
        }
        
        // 獲取下一個 token
        token = strtok(NULL, " ");
    }
    
    // 最終的中置表達式應該是堆疊中唯一的元素
    if(stack.top != 0){
        printf("無效的後綴表達式\n");
        free(expr);
        exit(1);
    }
    strcpy(infix, pop(&stack));
    
    // 釋放記憶體
    free(expr);
}

int main(){
    char postfix[MAX];
    // 讀取後綴表達式
    printf("請輸入後綴表達式（以空格分隔，例如 '12 3 + 4 *'）: ");
    fgets(postfix, MAX, stdin);
    
    // 移除末尾的換行符
    size_t len = strlen(postfix);
    if(len >0 && postfix[len-1] == '\n'){
        postfix[len-1] = '\0';
    }
    
    char infix[MAX];
    // 轉換後綴為中置
    postfixToInfixMultiDigit(postfix, infix);
    
    // 輸出中置表達式
    printf("中置表達式: %s\n", infix);
    
    return 0;
}
