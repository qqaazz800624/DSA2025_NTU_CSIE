#include <stdio.h>
#include <stdlib.h>

#define NIL (-1)

typedef struct {
    int  left, right, parent; 
    char color;               
    int  key;
} Node;

static Node *T;
static int root;

typedef struct { 
    int key, idx; 
} Pair;

static Pair *mapArr;
static int mapSize;

int cmp_pair(const void *a, const void *b);
int map_get(int key);
void left_rotate(int x);
void right_rotate(int y);

int main(void)
{
    int N;
    scanf("%d", &N);

    T = (Node*)malloc(sizeof(Node) * N);
    int *stk = (int*)malloc(sizeof(int) * N);  
    int top  = -1;

    for (int i = 0; i < N; ++i) {
        char c; int k;
        scanf(" %c %d", &c, &k);

        T[i].color = c;
        T[i].key   = k;
        T[i].left = T[i].right = T[i].parent = NIL;

        if (top == -1) {               
            root = i;
            stk[++top] = i;
        } else if (k < T[stk[top]].key) {   
            T[stk[top]].left = i;
            T[i].parent = stk[top];
            stk[++top] = i;
        } else {
            int last = NIL;
            while (top >= 0 && k > T[stk[top]].key){
                last = stk[top--];
            }
            T[last].right = i;
            T[i].parent = last;
            stk[++top] = i;
        }
    }
    free(stk);

    mapArr  = (Pair*)malloc(sizeof(Pair) * N);
    mapSize = N;
    for (int i = 0; i < N; ++i){
        mapArr[i].key = T[i].key;
        mapArr[i].idx = i;
    }
    qsort(mapArr, N, sizeof(Pair), cmp_pair);

    int M;  
    scanf("%d", &M);
    while (M--) {
        char op; int k;
        scanf(" %c %d", &op, &k);
        int idx = map_get(k);

        if (op == 'L') {
            left_rotate(idx);
        } else if (op == 'R') {
            right_rotate(idx);
        } else {   
            int LK, RK;
            if (T[idx].left == NIL) {
                LK = -1;
            } else {
                LK = T[T[idx].left].key;
            }
            if (T[idx].right == NIL) {
                RK = -1;
            } else {
                RK = T[T[idx].right].key;
            }                   

            int depth = 0, bdepth = 0;
            for (int p = T[idx].parent; p != NIL; p = T[p].parent) {
                ++depth;
                if (T[p].color == 'B') ++bdepth;
            }
            printf("%d %d %d %d\n", LK, RK, bdepth, depth);
        }
    }

    free(T);
    free(mapArr);
    return 0;
}

int cmp_pair(const void *a, const void *b) {
    const Pair *x = (const Pair*)a, *y = (const Pair*)b;
    return (x->key < y->key) ? -1 : (x->key > y->key);
}

int map_get(int key) {                 
    int lo = 0, hi = mapSize - 1;
    while (lo <= hi) {
        int mid = (lo + hi) >> 1;
        if (mapArr[mid].key == key){
            return mapArr[mid].idx;
        }
        if (key < mapArr[mid].key){
            hi  = mid - 1;
        } else {
            lo  = mid + 1;
        }
    }
    return NIL;                      
}

void left_rotate(int x) {
    int y = T[x].right;                 
    T[x].right = T[y].left;
    if (T[y].left != NIL){
        T[T[y].left].parent = x;
    } 

    T[y].parent = T[x].parent;
    if (T[x].parent == NIL){
        root = y;
    } else if (x == T[T[x].parent].left) {
        T[T[x].parent].left  = y;
    } else {
        T[T[x].parent].right = y;
    }

    T[y].left = x;
    T[x].parent = y;
}

void right_rotate(int y) {
    int x = T[y].left;
    T[y].left = T[x].right;
    if (T[x].right != NIL){
        T[T[x].right].parent = y;
    }

    T[x].parent = T[y].parent;
    if (T[y].parent == NIL){
        root = x;
    } else if (y == T[T[y].parent].left){
        T[T[y].parent].left  = x;
    } else{
        T[T[y].parent].right = x;
    }

    T[x].right = y;
    T[y].parent = x;
}