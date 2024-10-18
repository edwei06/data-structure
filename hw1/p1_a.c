#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define maximum size for stack and expressions
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
void push(Stack* stack, const char* expr, int precedence) {
    if (stack->top >= MAX - 1) {
        fprintf(stderr, "Stack Overflow\n");
        exit(EXIT_FAILURE);
    }
    stack->top++;
    strncpy(stack->data[stack->top].expr, expr, MAX - 1);
    stack->data[stack->top].expr[MAX - 1] = '\0'; // Ensure null-termination
    stack->data[stack->top].precedence = precedence;
}

// Pop an element from the stack
StackElement pop(Stack* stack) {
    if (isEmpty(stack)) {
        fprintf(stderr, "Invalid Prefix Expression\n");
        exit(EXIT_FAILURE);
    }
    return stack->data[stack->top--];
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
    return 3; // For operands and other characters
}

// Function to convert prefix to infix
char* prefixToInfix(const char* prefix) {
    Stack stack;
    initStack(&stack);
    int length = strlen(prefix);

    if (length == 0) {
        fprintf(stderr, "Empty Prefix Expression\n");
        exit(EXIT_FAILURE);
    }

    // Iterate from right to left
    for (int i = length - 1; i >= 0; i--) {
        char c = prefix[i];
        if (isalnum(c)) { // If operand, push to stack
            char operand[2] = {c, '\0'};
            push(&stack, operand, getPrecedence(c));
        }
        else if (isOperator(c)) { // If operator, pop two operands
            if (stack.top < 1) { // Ensure there are at least two operands
                fprintf(stderr, "Invalid Prefix Expression\n");
                exit(EXIT_FAILURE);
            }
            StackElement op1 = pop(&stack); // First operand (left)
            StackElement op2 = pop(&stack); // Second operand (right)
            char temp[MAX] = "";

            // Add parentheses if op1 has lower precedence
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

            // Add parentheses if op2 has lower precedence
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
            fprintf(stderr, "Invalid character in prefix expression: %c\n", c);
            exit(EXIT_FAILURE);
        }
    }

    // The final expression should be the only element in the stack
    if (stack.top != 0) {
        fprintf(stderr, "Invalid Prefix Expression\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the result
    char* result = (char*)malloc(strlen(stack.data[stack.top].expr) + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(result, stack.data[stack.top].expr);
    return result;
}

// Example usage
int main() {
    char prefix[MAX];

    // Read prefix expression without additional prompts
    if (scanf("%999s", prefix) != 1) { // Limit input to prevent buffer overflow
        fprintf(stderr, "Failed to read prefix expression.\n");
        return EXIT_FAILURE;
    }

    // Convert to infix
    char* infix = prefixToInfix(prefix);

    // Print infix expression
    printf("%s\n", infix);

    // Free allocated memory
    free(infix);

    return EXIT_SUCCESS;
}