/*
   Underground‑city network
   ------------------------
   ‑ mode 1 : articulation points     (“critical cities”)
   ‑ mode 2 : bridges                 (“critical tunnels”)
   ‑ mode 3 : biconnected components  (“independent exploration zones”)

   Limits : 1 ≤ N ≤ 1e5 , 1 ≤ M ≤ 1e6, simple connected graph
   Compile :  gcc main.c -O2 -std=c11 -fsanitize=address
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- basic graph (adjacency lists) ---------- */
typedef struct { int u , v; } Edge;              /* used for the DFS edge‑stack   */
typedef struct { int *v , sz; } Component;       /* stores one bi‑connected block */

static int N , M , mode;

/* adjacency */
static int  *head;          /* head[u] = first outgoing edge id (or -1)       */
static int  *nxt , *to;     /* nxt[id] = next edge id in list , to[id] = dst  */
static int   E = 0;         /* current edge id (goes 0 … 2M-1)                */
static void add_edge(int u,int v) {
    to[E] = v; nxt[E] = head[u]; head[u] = E++;
}

/* DFS bookkeeping */
static int  *disc , *low , *parent , timer = 0;
static char *is_art;
static long long bridge_cnt = 0;

/* edge stack for biconnected components */
static Edge *estk; int top = 0;

/* components */
static Component *comp; int comp_cnt = 0 , comp_cap = 0;

static void ensure_comp_capacity(void) {
    if (comp_cnt == comp_cap) {
        comp_cap = comp_cap ? comp_cap * 2 : 64;
        comp     = realloc(comp , comp_cap * sizeof(Component));
        if (!comp) { perror("realloc"); exit(1); }
    }
}

/* small helpers */
static int int_cmp(const void *a,const void *b){ return (*(const int*)a) - (*(const int*)b); }
static int comp_cmp(const void *A,const void *B)               /* lexicographic block ordering */
{
    const Component *x = (const Component*)A , *y = (const Component*)B;
    int m = x->sz < y->sz ? x->sz : y->sz;
    for (int i=0;i<m;i++)
        if (x->v[i] != y->v[i]) return x->v[i] - y->v[i];
    return x->sz - y->sz;
}

/* pop edges until (a,b) is removed and build one component */
static void extract_component(int a,int b)
{
    int cap = 32 , sz = 0;
    int *tmp = malloc(cap * sizeof(int));
    if (!tmp){ perror("malloc"); exit(1); }

    while (1) {
        Edge e = estk[--top];
        if (sz + 2 > cap) { cap <<= 1; tmp = realloc(tmp , cap*sizeof(int)); if(!tmp){perror("realloc");exit(1);} }
        tmp[sz++] = e.u;
        tmp[sz++] = e.v;
        if ((e.u==a && e.v==b) || (e.u==b && e.v==a)) break;
    }
    qsort(tmp , sz , sizeof(int) , int_cmp);

    /* unique */
    int uniq = 0;
    for (int i=0;i<sz;i++)
        if (i==0 || tmp[i]!=tmp[i-1])
            tmp[uniq++] = tmp[i];

    int *store = malloc(uniq * sizeof(int));
    if (!store){ perror("malloc"); exit(1); }
    memcpy(store , tmp , uniq*sizeof(int));
    free(tmp);

    ensure_comp_capacity();
    comp[comp_cnt].v  = store;
    comp[comp_cnt].sz = uniq;
    comp_cnt++;
}

/* depth‑first search (recursive Tarjan) */
static void dfs(int u)
{
    disc[u] = low[u] = ++timer;
    int child = 0;

    for (int id=head[u]; id!=-1; id=nxt[id]) {
        int v = to[id];
        if (!disc[v]) {                        /* tree edge */
            parent[v] = u;
            estk[top++] = (Edge){u,v};
            child++;
            dfs(v);

            if (low[v] < low[u]) low[u] = low[v];

            /* articulation test (u is root or non‑root) */
            if ((parent[u]==-1 && child>1) || (parent[u]!=-1 && low[v]>=disc[u]))
                is_art[u] = 1;

            /* bridge test */
            if (low[v] > disc[u]) bridge_cnt++;

            /* component boundary */
            if (low[v] >= disc[u]) extract_component(u,v);
        }
        else if (v != parent[u] && disc[v] < disc[u]) { /* back edge */
            estk[top++] = (Edge){u,v};
            if (disc[v] < low[u]) low[u] = disc[v];
        }
    }
}

int main(void)
{
    if (scanf("%d %d %d",&N,&M,&mode)!=3) return 0;

    /* allocate */
    head   = malloc(N * sizeof(int));
    disc   = calloc(N , sizeof(int));
    low    = malloc(N * sizeof(int));
    parent = malloc(N * sizeof(int));
    is_art = calloc(N , 1);

    nxt = malloc(2*M * sizeof(int));
    to  = malloc(2*M * sizeof(int));
    estk = malloc(2*M * sizeof(Edge));

    if(!head||!disc||!low||!parent||!is_art||!nxt||!to||!estk){ perror("malloc"); exit(1); }
    for (int i=0;i<N;i++) head[i] = -1 , parent[i]=-1;

    /* read graph */
    for (int i=0;i<M;i++){
        int u,v; scanf("%d %d",&u,&v);
        add_edge(u,v); add_edge(v,u);
    }

    dfs(0);   /* graph is connected, start anywhere */

    /* ---------- output ---------- */
    if (mode==1) {                       /* articulation points */
        int cnt = 0;
        for (int i=0;i<N;i++) if (is_art[i]) cnt++;
        printf("%d\n",cnt);
        for (int i=0,first=1;i<N;i++) if (is_art[i]) {
            if (!first) putchar(' ');
            printf("%d",i);
            first=0;
        }
        putchar('\n');
    }
    else if (mode==2) {                  /* bridges */
        printf("%lld\n",bridge_cnt);
    }
    else {                               /* biconnected components */
        qsort(comp , comp_cnt , sizeof(Component) , comp_cmp);
        printf("%d\n",comp_cnt);
        for (int i=0;i<comp_cnt;i++){
            for (int j=0;j<comp[i].sz;j++){
                if (j) putchar(' ');
                printf("%d",comp[i].v[j]);
            }
            putchar('\n');
        }
    }

    /* free (optional in contest setting) */
    for (int i=0;i<comp_cnt;i++) free(comp[i].v);
    free(comp); free(estk); free(to); free(nxt);
    free(is_art); free(parent); free(low); free(disc); free(head);
    return 0;
}
