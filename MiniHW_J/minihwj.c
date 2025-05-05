#include <stdio.h>
#include <stdlib.h>

#define MAXN 1000001
int parent[MAXN];
int rank[MAXN];

void Make_Set(int x);
int Find_Set(int x);
void Link(int x, int y);
void Union(int x, int y);
void Print_Path(int x);

int main(){
    int N, M;
    scanf("%d", &N);

    for (int i = 1; i <= N; i++){
        Make_Set(i);
    }
    scanf("%d", &M);

    char command;
    int x, y;
    for (int i = 0; i < M; i++){
        scanf(" %c", &command);
        if (command == 'F'){
            scanf("%d", &x);
            Find_Set(x);
        } else if (command == 'U'){
            scanf("%d %d", &x, &y);
            Union(x, y);
        } else if (command == 'P'){
            scanf("%d", &x);
            Print_Path(x);
        }
    }

    return 0;
}

void Make_Set(int x){
    parent[x] = x;
    rank[x] = 0;
}

int Find_Set(int x){
    if (x != parent[x]){
        parent[x] = Find_Set(parent[x]); 
    }
    return parent[x];
}

void Link(int x, int y){
    if (rank[x] > rank[y]){
        parent[y] = x;    
    } else {
        parent[x] = y;
        if (rank[x] == rank[y]){
            rank[y]++;
        }
    }
}

void Union(int x, int y){
    int root_x = Find_Set(x);
    int root_y = Find_Set(y);
    if (root_x != root_y){
        Link(root_x, root_y);
    }
}

void Print_Path(int x){
    int path_list[32];
    int last = 0;

    while (parent[x] != x){
        path_list[last++] = x;
        x = parent[x];
    }
    path_list[last++] = x;
    
    for (int i = 0; i < last; i++){
        printf("%d", path_list[i]);
        if (i != last - 1){
            printf(" ");
        }
    }
    printf("\n");
}