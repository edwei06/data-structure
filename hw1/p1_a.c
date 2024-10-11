#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define maximum size for stack
#define MAX 1000

// Structure to hold expression and its precedence
typedef struct {
    char expr[MAX];
    int precedence;
} StackElement;

// Stack structure
typedef struct {
    StackElement data[MAX];
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
void push(Stack* stack, char* expr, int precedence) {
    if (stack->top >= MAX - 1) {
        printf("Stack Overflow\n");
        exit(1);
    }
    stack->top++;
    strcpy(stack->data[stack->top].expr, expr);
    stack->data[stack->top].precedence = precedence;
}

// Pop an element from the stack
StackElement pop(Stack* stack) {
    if (isEmpty(stack)) {
        printf("Invalid Prefix Expression\n");
        exit(1);
    }
    StackElement elem = stack->data[stack->top];
    stack->top--;
    return elem;
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
    return 3; // For operands
}

// Function to convert prefix to infix
char* prefixToInfix(char* prefix) {
    Stack stack;
    initStack(&stack);
    int length = strlen(prefix);
    // Iterate from right to left
    for (int i = length - 1; i >= 0; i--) {
        char c = prefix[i];
        if (isalnum(c)) { // If operand, push to stack
            char operand[2] = {c, '\0'};
            push(&stack, operand, getPrecedence(c));
        }
        else if (isOperator(c)) { // If operator, pop two operands
            StackElement op1 = pop(&stack);
            StackElement op2 = pop(&stack);
            char temp[MAX] = "";
            // Add parentheses if needed based on precedence
            // If op1 has lower precedence, add parentheses
            if (op1.precedence < getPrecedence(c)) {
                strcat(temp, "(");
                strcat(temp, op1.expr);
                strcat(temp, ")");
            }
            else {
                strcat(temp, op1.expr);
            }
            // Add operator
            int len = strlen(temp);
            temp[len] = c;
            temp[len + 1] = '\0';
            // If op2 has lower precedence, add parentheses
            if (op2.precedence < getPrecedence(c)) {
                strcat(temp, "(");
                strcat(temp, op2.expr);
                strcat(temp, ")");
            }
            else {
                strcat(temp, op2.expr);
            }
            // Push the new expression back to stack
            push(&stack, temp, getPrecedence(c));
        }
        else {
            // Invalid character
            printf("Invalid character in prefix expression: %c\n", c);
            exit(1);
        }
    }
    // The final expression is on top of the stack
    if (stack.top != 0) {
        printf("Invalid Prefix Expression\n");
        exit(1);
    }
    return stack.data[stack.top].expr;
}

// Example usage
int main() {
    char prefix[100];
    // Read prefix expression
    scanf("%s", prefix);
    // Convert to infix
    char* infix = prefixToInfix(prefix);
    // Print infix expression
    printf("%s\n", infix);
    return 0;
}
