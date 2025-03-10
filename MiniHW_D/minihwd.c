#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    int data;
    int next;
} node;

void FLOYD_CYCLE_DETECTION(node *nodes, int N);

int main(){

    int N;
    scanf("%d", &N);

    int i;
    node *nodes;
    nodes = (node *)malloc((N + 1) * sizeof(node));
    for (i = 1; i <= N; i++) {
        scanf("%d %d", &nodes[i].data, &nodes[i].next);
    }

    FLOYD_CYCLE_DETECTION(nodes, N);
    
    free(nodes);

    return 0;
}

void FLOYD_CYCLE_DETECTION(node *nodes, int N){

    int *visited;
    visited = (int *)malloc((N + 1) * sizeof(int));
    
    int i;
    int vcount = 0;
    int hare = 1, tortoise = 1;
    visited[vcount++] = nodes[1].data;

    while (hare != 0 && nodes[hare].next != 0){
        int next1 = nodes[hare].next;
        int next2 = nodes[next1].next;
        hare = next2;
        tortoise = nodes[tortoise].next;
        if (hare == 0){
            break;
        }
        visited[vcount++] = nodes[hare].data;
        if (hare == tortoise){
            break;
        }
    }

    for (i = 0; i < vcount; i++) {
        printf("%d", visited[i]);
        if (i < vcount - 1) {
            printf(" ");
        }
    }
    printf("\n");

    free(visited);
}
