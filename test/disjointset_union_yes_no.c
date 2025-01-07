#include <stdio.h>
#include <string.h>

#define MAXN 500000

int parent[MAXN + 1];
int rank[MAXN + 1];

// Find the representative of the set containing x
int find(int x) {
    if (parent[x] != x) {
        parent[x] = find(parent[x]); // Path compression
    }
    return parent[x];
}

// Union of two sets containing x and y
void union_sets(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);

    if (rootX != rootY) {
        // Union by rank
        if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
    }
}

int main() {
    int n, m;
    scanf("%d %d", &n, &m);

    // Initialize DSU
    for (int i = 1; i <= n; i++) {
        parent[i] = i;
        rank[i] = 0;
    }

    char query[10];
    int u, v;

    for (int i = 0; i < m; i++) {
        scanf("%s %d %d", query, &u, &v);

        if (strcmp(query, "union") == 0) {
            union_sets(u, v);
        } else if (strcmp(query, "get") == 0) {
            if (find(u) == find(v)) {
                printf("YES\n");
            } else {
                printf("NO\n");
            }
        }
    }

    return 0;
}
