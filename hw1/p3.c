#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Define the maximum number of vertices
#define MAX_VERTICES 1000

// Structure to represent a queue
typedef struct {
    int items[MAX_VERTICES];
    int front;
    int rear;
} Queue;

// Function to initialize the queue
void initializeQueue(Queue* q) {
    q->front = -1;
    q->rear = -1;
}

// Function to check if the queue is empty
bool isEmpty(Queue* q) {
    return (q->front == -1);
}

// Function to enqueue an element
void enqueue(Queue* q, int value) {
    if (q->rear == MAX_VERTICES - 1) {
        printf("Queue Overflow\n");
        exit(1);
    }
    if (isEmpty(q)) {
        q->front = 0;
    }
    q->rear++;
    q->items[q->rear] = value;
}

// Function to dequeue an element
int dequeue(Queue* q) {
    if (isEmpty(q)) {
        printf("Queue Underflow\n");
        exit(1);
    }
    int item = q->items[q->front];
    q->front++;
    // Reset the queue if it becomes empty
    if (q->front > q->rear) {
        initializeQueue(q);
    }
    return item;
}

// Function to perform BFS
void BFS(int m, int adjacencyMatrix[][MAX_VERTICES], int startVertex) {
    bool visited[MAX_VERTICES] = {false};
    Queue q;
    initializeQueue(&q);

    // Enqueue the starting vertex and mark it as visited
    enqueue(&q, startVertex);
    visited[startVertex] = true;

    while (!isEmpty(&q)) {
        int currentVertex = dequeue(&q);
        printf("%d ", currentVertex);

        // Visit all adjacent vertices in ascending order
        for (int v = 1; v <= m; v++) {
            if (adjacencyMatrix[currentVertex][v] == 1 && !visited[v]) {
                enqueue(&q, v);
                visited[v] = true;
            }
        }
    }
}

int main() {
    int m;
    // Read the number of vertices
    if (scanf("%d", &m) != 1) {
        printf("Invalid input for number of vertices.\n");
        return 1;
    }

    // Declare the adjacency matrix (1-based indexing)
    int adjacencyMatrix[MAX_VERTICES][MAX_VERTICES];
    // Initialize the adjacency matrix to 0
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= m; j++) {
            adjacencyMatrix[i][j] = 0;
        }
    }

    // Read the adjacency matrix
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= m; j++) {
            if (scanf("%d", &adjacencyMatrix[i][j]) != 1) {
                printf("Invalid input for adjacency matrix.\n");
                return 1;
            }
        }
    }

    // Perform BFS starting from vertex 1
    BFS(m, adjacencyMatrix, 1);
    printf("\n"); // Newline after BFS output

    return 0;
}