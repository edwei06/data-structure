#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define maximum size for stack
#define MAX 1000

// Stack structure to hold operators
typedef struct {
    char data[MAX];
    int top;
} Stack;

// Initialize the stack
void initStack(Stack* stack) {
    stack->top = -1;
}

// Check if the stack is empty
int isEmpty(Stack* stack) {
    return stack->top == -1;
}

// Push an element onto the stack
void push(Stack* stack, char c) {
    if (stack->top >= MAX - 1) {
        printf("Stack Overflow\n");
        exit(1);
    }
    stack->data[++(stack->top)] = c;
}

// Pop an element from the stack
char pop(Stack* stack) {
    if (isEmpty(stack)) {
        printf("Invalid Infix Expression\n");
        exit(1);
    }
    return stack->data[(stack->top)--];
}

// Function to check if a character is an operator
int isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

// Function to get precedence of operators
int getPrecedence(char c) {
    if (c == '+' || c == '-')
        return 1;
    if (c == '*' || c == '/')
        return 2;
    return 0; // For operands
}

// Function to convert infix to postfix
void infixToPostfix(char* infix, char* postfix) {
    Stack stack;
    initStack(&stack);
    int i, k = 0;
    for (i = 0; i < strlen(infix); i++) {
        char c = infix[i];
        
        // If operand, add to postfix
        if (isalnum(c)) {
            postfix[k++] = c;
        }
        // If operator
        else if (isOperator(c)) {
            while (!isEmpty(&stack) && getPrecedence(stack.data[stack.top]) >= getPrecedence(c)) {
                postfix[k++] = pop(&stack);
            }
            push(&stack, c);
        }
        else {
            // Since the test data doesn't contain parentheses or spaces, any other character is invalid
            printf("Invalid character in infix expression: %c\n", c);
            exit(1);
        }
    }
    
    // Pop all remaining operators from the stack
    while (!isEmpty(&stack)) {
        char op = pop(&stack);
        // If there's an unexpected parenthesis, which shouldn't occur as per problem statement
        if (op == '(' || op == ')') {
            printf("Invalid Infix Expression\n");
            exit(1);
        }
        postfix[k++] = op;
    }
    
    postfix[k] = '\0'; // Null-terminate the postfix string
}

// Example usage
int main() {
    char infix[100];
    char postfix[100];
    
    // Read infix expression
    scanf("%s", infix);
    
    // Convert to postfix
    infixToPostfix(infix, postfix);
    
    // Print postfix expression
    printf("%s\n", postfix);
    
    return 0;
}
