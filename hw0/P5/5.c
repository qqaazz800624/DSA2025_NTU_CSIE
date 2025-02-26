#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_N 20
#define INF 100

int edges[MAX_N][MAX_N]; 
int bestCover[MAX_N], bestSize = INF;

int isValidCover(int subset, int n);
void updateBestCover(int subset, int n);
void findMinimumVertexCover(int n);

int main(){
    int n, m;
    int i, j;
    scanf("%d %d", &n, &m);

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            edges[i][j] = 0;
        }
    }

    int u, v;
    for (i = 0; i < m; i++) {
        scanf("%d %d", &u, &v);
        edges[u][v] = edges[v][u] = 1;
    }

    findMinimumVertexCover(n);

    for (i = 0; i < bestSize; i++){
        printf("%d\n", bestCover[i]);
    }

    return 0;
}

int isValidCover(int subset, int n){
    int u, v;
    for (u = 0; u < n; u++){
        for (v = u + 1; v < n; v++){
            if (edges[u][v]){
                if (!((subset & (1 << u)) || (subset & (1 << v)))){
                    return 0;
                }
            }
        }
    }
    return 1;
}

void updateBestCover(int subset, int n){
    int count = 0;
    int tempCover[MAX_N];
    int i, j;

    for (i = 0; i < n; i++){
        if (subset & (1 << i)){
            tempCover[count++] = i;
        }
    }

    if (count < bestSize){
        bestSize = count;
        for (i = 0; i < count; i++){
            bestCover[i] = tempCover[i];
        }
    } else if (count == bestSize){
        for (i = 0; i < count; i++){
            if (tempCover[i] < bestCover[i]){
                for (j = 0; j < count; j++){
                    bestCover[j] = tempCover[j];
                }
                break;
            } else if (tempCover[i] > bestCover[i]){
                break;
            }
        }
    }
}

void findMinimumVertexCover(int n){
    int totalSubsets = (1 << n);
    int subset;
    for (subset = 0; subset < totalSubsets; subset++) {
        if (isValidCover(subset, n)) {
            updateBestCover(subset, n);
        }
    }
}