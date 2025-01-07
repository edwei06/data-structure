#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of elements and queries
#define MAXN 300005

// Union-Find structure arrays
int parent[MAXN];
int size_set[MAXN];
int min_set[MAXN];
int max_set[MAXN];

// Function to find the representative of a set with path compression
int find_set(int x) {
    if (parent[x] != x)
        parent[x] = find_set(parent[x]);
    return parent[x];
}

// Function to union two sets by size and update min and max
void union_set(int u, int v) {
    int pu = find_set(u);
    int pv = find_set(v);
    if (pu == pv)
        return;
    // Union by size: attach smaller set to larger set
    if (size_set[pu] < size_set[pv]) {
        int temp = pu;
        pu = pv;
        pv = temp;
    }
    parent[pv] = pu;
    size_set[pu] += size_set[pv];
    if (min_set[pv] < min_set[pu])
        min_set[pu] = min_set[pv];
    if (max_set[pv] > max_set[pu])
        max_set[pu] = max_set[pv];
}

int main() {
    int n, m;
    // Fast input reading
    if (scanf("%d %d", &n, &m) != 2) {
        return 0;
    }

    // Initialize each element to be its own parent, size 1, min and max equal to itself
    for (int i = 1; i <= n; ++i) {
        parent[i] = i;
        size_set[i] = 1;
        min_set[i] = i;
        max_set[i] = i;
    }

    // Prepare output buffer
    // Each "get" query can produce up to 20 characters (including spaces and newline)
    // For m=3e5, total buffer size = 20 * 3e5 = 6e6
    // Allocate a buffer slightly larger to be safe
    char *outbuf = (char *)malloc(7000000 * sizeof(char));
    if (!outbuf) {
        return 1; // Allocation failed
    }
    int out_ptr = 0;

    char cmd[10];
    int u, v;
    for (int i = 0; i < m; ++i) {
        if (scanf("%s", cmd) != 1)
            break;
        if (cmd[0] == 'u') { // "union" command
            if (scanf("%d %d", &u, &v) != 2)
                break;
            union_set(u, v);
        }
        else if (cmd[0] == 'g') { // "get" command
            if (scanf("%d", &v) != 1)
                break;
            int p = find_set(v);
            // Convert integers to string and append to outbuf
            // Using sprintf to write to the buffer
            // Keep track of the current position in outbuf
            out_ptr += sprintf(outbuf + out_ptr, "%d %d %d\n", min_set[p], max_set[p], size_set[p]);
        }
    }

    // Write all outputs at once
    fwrite(outbuf, sizeof(char), out_ptr, stdout);

    // Free the allocated buffer
    free(outbuf);

    return 0;
}
