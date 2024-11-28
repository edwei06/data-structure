#include <stdio.h>
#include <stdlib.h>

#define MAX_NODES 100005
#define MAX_EDGES 200005
#define MAX_HEAP 400010
#define INF 1000000000

typedef struct Edge {
    int to;
    int weight;
    struct Edge* next;
} Edge;

typedef struct HeapElement {
    int node;
    long weight;
} HeapElement;

// Global variables
Edge edges_array[MAX_EDGES * 2];
int edge_count = 0;

Edge* adj[MAX_NODES];

HeapElement heap_array[MAX_HEAP];
int heap_size = 0;

// Function to push an element into the heap
void heap_push(int node, long weight) {
    if (heap_size >= MAX_HEAP) {
        fprintf(stderr, "Heap overflow\n");
        exit(1);
    }
    heap_array[heap_size].node = node;
    heap_array[heap_size].weight = weight;
    int i = heap_size;
    heap_size++;
    
    // Percolate up
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap_array[i].weight < heap_array[parent].weight) {
            // Swap
            HeapElement temp = heap_array[i];
            heap_array[i] = heap_array[parent];
            heap_array[parent] = temp;
            i = parent;
        }
        else {
            break;
        }
    }
}

// Function to pop the minimum element from the heap
HeapElement heap_pop_min() {
    if (heap_size <= 0) {
        fprintf(stderr, "Heap underflow\n");
        exit(1);
    }
    HeapElement min = heap_array[0];
    heap_size--;
    heap_array[0] = heap_array[heap_size];
    
    // Percolate down
    int i = 0;
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;
        
        if (left < heap_size && heap_array[left].weight < heap_array[smallest].weight) {
            smallest = left;
        }
        if (right < heap_size && heap_array[right].weight < heap_array[smallest].weight) {
            smallest = right;
        }
        if (smallest != i) {
            // Swap
            HeapElement temp = heap_array[i];
            heap_array[i] = heap_array[smallest];
            heap_array[smallest] = temp;
            i = smallest;
        }
        else {
            break;
        }
    }
    
    return min;
}

int main() {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) {
        fprintf(stderr, "Error reading n and m\n");
        return 1;
    }
    
    // Initialize adjacency list
    for (int u = 1; u <= n; u++) {
        adj[u] = NULL;
    }
    
    // Read m edges
    for (int i = 0; i < m; i++) {
        int u, v, w;
        if (scanf("%d %d %d", &u, &v, &w) != 3) {
            fprintf(stderr, "Error reading edge %d\n", i+1);
            return 1;
        }
        
        // Add edge u -> v
        edges_array[edge_count].to = v;
        edges_array[edge_count].weight = w;
        edges_array[edge_count].next = adj[u];
        adj[u] = &edges_array[edge_count];
        edge_count++;
        
        // Add edge v -> u
        edges_array[edge_count].to = u;
        edges_array[edge_count].weight = w;
        edges_array[edge_count].next = adj[v];
        adj[v] = &edges_array[edge_count];
        edge_count++;
    }
    
    // Read starting node
    int S;
    if (scanf("%d", &S) != 1) {
        fprintf(stderr, "Error reading starting node\n");
        return 1;
    }
    
    // Initialize variables for Prim's algorithm
    long total_weight = 0;
    int* visited = (int*)calloc(n + 1, sizeof(int));
    if (visited == NULL) {
        fprintf(stderr, "Memory allocation failed for visited array\n");
        return 1;
    }
    
    // Push the starting node with weight 0
    heap_push(S, 0);
    
    // Prim's algorithm
    while (heap_size > 0) {
        HeapElement current = heap_pop_min();
        int u = current.node;
        long w = current.weight;
        
        if (visited[u]) {
            continue;
        }
        
        visited[u] = 1;
        total_weight += w;
        
        // Iterate through all adjacent edges
        Edge* edge_ptr = adj[u];
        while (edge_ptr != NULL) {
            int v = edge_ptr->to;
            int edge_w = edge_ptr->weight;
            if (!visited[v]) {
                heap_push(v, edge_w);
            }
            edge_ptr = edge_ptr->next;
        }
    }
    
    // Free allocated memory
    free(visited);
    
    // Print the total weight of the minimum spanning tree
    printf("%ld\n", total_weight);
    
    return 0;
}
