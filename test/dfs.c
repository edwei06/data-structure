#include <stdio.h>
#include <stdlib.h>

#define MAX_N 1001  // Maximum number of vertices (1 ≤ N ≤ 1000)

int N, M, V;  // Number of vertices, number of edges, starting vertex
int adj[MAX_N][MAX_N];  // Adjacency matrix to store edges (1-based indexing)
int adj_size[MAX_N];    // Number of adjacent vertices for each vertex
int visited_dfs[MAX_N]; // Visited array for DFS
int visited_bfs[MAX_N]; // Visited array for BFS

// Comparison function for qsort (ascending order)
int cmpfunc(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// DFS function
void dfs(int u) {
    visited_dfs[u] = 1;
    printf("%d ", u);
    for (int i = 0; i < adj_size[u]; ++i) {
        int v = adj[u][i];
        if (!visited_dfs[v]) {
            dfs(v);
        }
    }
}

// BFS function
void bfs(int start) {
    int queue[MAX_N];
    int front = 0, rear = 0;
    visited_bfs[start] = 1;
    queue[rear++] = start;

    while (front < rear) {
        int u = queue[front++];
        printf("%d ", u);
        for (int i = 0; i < adj_size[u]; ++i) {
            int v = adj[u][i];
            if (!visited_bfs[v]) {
                visited_bfs[v] = 1;
                queue[rear++] = v;
            }
        }
    }
}

int main() {
    scanf("%d %d %d", &N, &M, &V);

    // Initialize adjacency matrix and adjacency list sizes
    for (int i = 1; i <= N; ++i) {
        adj_size[i] = 0;
    }

    // Read edges and build adjacency lists
    for (int i = 0; i < M; ++i) {
        int u, v;
        scanf("%d %d", &u, &v);

        // Avoid adding duplicate edges
        int duplicate = 0;
        for (int j = 0; j < adj_size[u]; ++j) {
            if (adj[u][j] == v) {
                duplicate = 1;
                break;
            }
        }
        if (!duplicate) {
            adj[u][adj_size[u]++] = v;
            adj[v][adj_size[v]++] = u;
        }
    }

    // Sort adjacency lists
    for (int i = 1; i <= N; ++i) {
        qsort(adj[i], adj_size[i], sizeof(int), cmpfunc);
    }

    // Perform DFS
    dfs(V);
    printf("\n");

    // Perform BFS
    bfs(V);
    printf("\n");

    return 0;
}
