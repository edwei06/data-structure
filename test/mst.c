#include <stdio.h>
#include <stdlib.h>

// Define the maximum number of nodes based on constraints
#define MAX_NODES 10005

// Structure to represent an edge
typedef struct {
    int u;
    int v;
    int w;
} Edge;

// Comparator function for qsort to sort edges by weight in ascending order
int cmp(const void* a, const void* b) {
    Edge* ea = (Edge*)a;
    Edge* eb = (Edge*)b;
    if (ea->w < eb->w) return -1;
    else if (ea->w > eb->w) return 1;
    else return 0;
}

// Arrays for Disjoint Set Union (DSU)
int parent[MAX_NODES];
int rank_[MAX_NODES];

// Function to find the set representative with path compression
int find_set(int x) {
    if (parent[x] != x)
        parent[x] = find_set(parent[x]);
    return parent[x];
}

// Function to union two sets by rank
void union_set(int x, int y) {
    int fx = find_set(x);
    int fy = find_set(y);
    if (fx == fy) return;
    if (rank_[fx] < rank_[fy]) {
        parent[fx] = fy;
    }
    else {
        parent[fy] = fx;
        if (rank_[fx] == rank_[fy])
            rank_[fx]++;
    }
}

int main(){
    int N, M;
    
    // Read number of nodes and edges
    if (scanf("%d %d", &N, &M) != 2){
        return 1; // Input error
    }

    // Allocate memory for edges
    Edge* edges = (Edge*)malloc(M * sizeof(Edge));
    if (edges == NULL){
        return 1; // Memory allocation failed
    }

    // Read all edges
    for(int i=0; i<M; i++){
        if (scanf("%d %d %d", &edges[i].u, &edges[i].v, &edges[i].w) != 3){
            free(edges);
            return 1; // Input error
        }
    }

    // Sort edges by weight
    qsort(edges, M, sizeof(Edge), cmp);

    // Initialize DSU
    for(int i=1; i<=N; i++){
        parent[i] = i;
        rank_[i] = 0;
    }

    long long total = 0;
    int count = 0; // To keep track of number of edges added to MST

    // Iterate through sorted edges
    for(int i=0; i<M && count < N-1; i++){
        int u = edges[i].u;
        int v = edges[i].v;
        int set_u = find_set(u);
        int set_v = find_set(v);
        if(set_u != set_v){
            total += edges[i].w;
            union_set(u, v);
            count++;
        }
    }

    // Free allocated memory
    free(edges);

    // Print the total weight of the MST
    printf("%lld", total);

    return 0;
}
