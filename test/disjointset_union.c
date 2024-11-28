#include <stdio.h>

// Maximum number of elements
#define MAX 300005

// Disjoint Set Union (DSU) structure
int parent_set[MAX];
int size_set[MAX];
int min_elem[MAX];
int max_elem_set[MAX];

// Find the root of the set containing x with path compression
int find_set(int x) {
    if (parent_set[x] != x) {
        parent_set[x] = find_set(parent_set[x]);
    }
    return parent_set[x];
}

// Union the sets containing x and y
void union_sets(int x, int y) {
    int root_x = find_set(x);
    int root_y = find_set(y);
    
    if (root_x == root_y)
        return;
    
    // Union by size to keep tree shallow
    if (size_set[root_x] < size_set[root_y]) {
        parent_set[root_x] = root_y;
        size_set[root_y] += size_set[root_x];
        
        // Update minimum and maximum elements
        if (min_elem[root_x] < min_elem[root_y])
            min_elem[root_y] = min_elem[root_x];
        if (max_elem_set[root_x] > max_elem_set[root_y])
            max_elem_set[root_y] = max_elem_set[root_x];
    }
    else {
        parent_set[root_y] = root_x;
        size_set[root_x] += size_set[root_y];
        
        // Update minimum and maximum elements
        if (min_elem[root_y] < min_elem[root_x])
            min_elem[root_x] = min_elem[root_y];
        if (max_elem_set[root_y] > max_elem_set[root_x])
            max_elem_set[root_x] = max_elem_set[root_y];
    }
}

int main() {
    int n, m;
    
    // Fast IO
    scanf("%d %d", &n, &m);
    
    // Initialize DSU
    for(int i = 1; i <= n; i++) {
        parent_set[i] = i;
        size_set[i] = 1;
        min_elem[i] = i;
        max_elem_set[i] = i;
    }
    
    char cmd[6];
    int u, v;
    
    for(int i = 0; i < m; i++) {
        scanf("%s", cmd);
        if(cmd[0] == 'u') { // "union" command
            scanf("%d %d", &u, &v);
            union_sets(u, v);
        }
        else { // "get" command
            scanf("%d", &v);
            int root = find_set(v);
            printf("%d %d %d\n", min_elem[root], max_elem_set[root], size_set[root]);
        }
    }
    
    return 0;
}
