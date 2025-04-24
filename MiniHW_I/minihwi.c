#include <stdio.h>
#include <stdlib.h>

#define MAXV 1000

enum { WHITE = 0, GRAY = 1, BLACK = 2 };

int n;
int deg[MAXV+1];
static int adj[MAXV+1][MAXV];     
static char cls[MAXV+1][MAXV+1];  

int color[MAXV+1];
int pi[MAXV+1];
int dtime[MAXV+1], ftime[MAXV+1];
int timer = 0;

void DFS_Visit(int u);
void DFS(int n);

int main() {
    
    int n;
    scanf("%d", &n);
    for (int u = 1; u <= n; u++) {
        scanf("%d", &deg[u]);
        for (int j = 0; j < deg[u]; j++) {
            scanf("%d", &adj[u][j]);
        }
    }

    DFS(n);

    for (int u = 1; u <= n; u++) {
        printf("%d", u);
        if (deg[u] > 0) {
            putchar(' ');
            for (int i = 0; i < deg[u]; i++) {
                putchar(cls[u][i]);
            }
        }
        putchar('\n');
    }

    return 0;
}

void DFS_Visit(int u) {
    dtime[u] = ++timer;
    color[u] = GRAY;

    for (int i = 0; i < deg[u]; i++) {
        int v = adj[u][i];
        if (color[v] == WHITE) {
            pi[v] = u;
            cls[u][i] = 'T';            
            DFS_Visit(v);
        }
        else if (color[v] == GRAY) {
            cls[u][i] = 'B';            
        }
        else { 
            if (dtime[u] < dtime[v])
                cls[u][i] = 'F';        
            else
                cls[u][i] = 'C';       
        }
    }

    color[u] = BLACK;
    ftime[u] = ++timer;
}

void DFS(int n){
    for (int u = 1; u <= n; u++) {
        color[u] = WHITE;
        pi[u] = -1;
    }
    timer = 0;

    for (int u = 1; u <= n; u++) {
        if (color[u] == WHITE) {
            DFS_Visit(u);
        }
    }
}