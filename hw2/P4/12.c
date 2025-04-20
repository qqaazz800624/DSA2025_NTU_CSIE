#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAXN 300005
#define MAX_LOG 20

typedef unsigned long long ull;

int c[MAXN];     
ull ls[MAXN];         
int student_slot[MAXN];  
int head[MAXN], to[2*MAXN], nxt[2*MAXN], w[2*MAXN], eidx;
int N;
long long bit[MAXN];
int up[MAXN][MAX_LOG], depth[MAXN];
long long dist0[MAXN];    
int tin[MAXN], tout[MAXN], timer;
int slot_size[MAXN];
struct pos {
    int s;       
    ull num, den;
};
struct pos slots[MAXN][30];
struct notif {
    ull t;
    int s;
};
struct notif all_notifs[600000];
int notif_count = 0;
int fetched_ptr = 0;

void add_edge(int u, int v, int ww);
void dfs(int v, int p);
int cmp_pos(const void *aa, const void *bb);
void insert_bike(int s, int x, ull pnum, ull pden, int output);
ull gcd(ull a, ull b);
void park_bike(int s, int x, int p, int output);
int lca(int a, int b);
long long bit_sum(int i);
long long currDist(int v);
long long get_dist(int a, int b);
void insert_notif(ull t, int s);
void do_fetch(ull T);
void bit_update(int i, long long v);

int main(){

    int n, m, q; // number of slots, students, queries
    scanf("%d%d%d", &n, &m, &q);

    for (int i = 0; i < n; i++) {
        scanf("%d", &c[i]);
    }

    memset(student_slot, -1, m * sizeof student_slot[0]);
    for (int i = 0; i < m; i++) {
        scanf("%llu", &ls[i]);
    }

    memset(head, -1, sizeof head);
    eidx = 0;
    for (int i = 0, u, v, ww; i < n - 1; i++) {
        scanf("%d%d%d", &u, &v, &ww);
        add_edge(u, v, ww);
    }

    N = n;
    memset(bit,0,sizeof(long long)*(n+2));
    timer=0;
    depth[0]=0; dist0[0]=0;
    dfs(0,0);

    for (int i = 0; i < q; i++){
        int op;
        scanf("%d", &op);
        if (op == 0){
            int s,x,p;
            scanf("%d%d%d", &s, &x, &p);
            park_bike(s, x, p, 1);
        } else if (op == 1){
            int s,x,p; scanf("%d%d%d",&s,&x,&p);
            int src = student_slot[s];
            if(src==x){
                printf("%d moved to %d in 0 seconds.\n", s, x);
                continue;
            }
            
            if(src!=-1){
                for(int j=0;j<slot_size[src];j++){
                    if(slots[src][j].s==s){
                        for(int k=j;k+1<slot_size[src];k++)
                            slots[src][k]=slots[src][k+1];
                        slot_size[src]--;
                        break;
                    }
                }
            }
            park_bike(s,x,p,0);
            long long tc = (src==-1?0:get_dist(src,x));
            printf("%d moved to %d in %lld seconds.\n",s,x,tc);
        } else if (op == 2){
            int x; ull T; 
            scanf("%d%llu", &x, &T);
            for(int j=0;j<slot_size[x];j++){
                int st = slots[x][j].s;
                if(student_slot[st]==x){
                    insert_notif(T + ls[st], st);
                    student_slot[st] = -1;
                }
            }
            slot_size[x]=0;
        } else if (op == 3){
            int x; ull T; 
            scanf("%d%llu",&x,&T);
            int cnt=0, nc=0;
            struct pos tmp[30];

            for(int j=0;j<slot_size[x];j++){
                int st=slots[x][j].s;
                if(slots[x][j].den>1 && student_slot[st]==x){
                    insert_notif(T + ls[st], st);
                    student_slot[st] = -1;
                    cnt++;
                } else {
                    tmp[nc++] = slots[x][j];
                }
            }

            for(int j=0;j<nc;j++){
                slots[x][j]=tmp[j];
            }

            slot_size[x]=nc;
            printf("Rearranged %d bikes in %d.\n",cnt,x);
        } else if (op == 4){
            ull T; scanf("%llu",&T);
            do_fetch(T);
        } else if (op == 5){
            int x,y,d; 
            scanf("%d%d%d",&x,&y,&d);
            
            long long oldw=0;
            for(int e=head[x];e!=-1;e=nxt[e]){
                if(to[e]==y){
                    oldw=w[e]; 
                    break; 
                }
            }
            long long delta = (long long)d - oldw;
            
            for(int e=head[x];e!=-1;e=nxt[e]){
                if(to[e]==y){
                    w[e]=d;
                }
            }

            for(int e=head[y];e!=-1;e=nxt[e]){
                if(to[e]==x){
                    w[e]=d;
                }
            }
            
            int child = (depth[x]>depth[y] ? x : y);
            
            bit_update(tin[child], delta);
            if (tout[child]+1 <= N){
                bit_update(tout[child]+1, -delta);
            }
        }
    }

    return 0;
}

void add_edge(int u, int v, int ww) {
    to[eidx] = v; w[eidx] = ww; nxt[eidx] = head[u]; head[u] = eidx++;
    to[eidx] = u; w[eidx] = ww; nxt[eidx] = head[v]; head[v] = eidx++;
}

void dfs(int v, int p) {
    tin[v] = ++timer;
    up[v][0] = p;
    for (int k = 1; k < MAX_LOG; k++)
        up[v][k] = up[ up[v][k-1] ][k-1];
    for (int e = head[v]; e != -1; e = nxt[e]) {
        int u = to[e];
        if (u == p) continue;
        depth[u] = depth[v] + 1;
        dist0[u] = dist0[v] + w[e];
        dfs(u, v);
    }
    tout[v] = timer;
}

int cmp_pos(const void *aa, const void *bb) {
    const struct pos *A = aa, *B = bb;
    __uint128_t lhs = (__uint128_t)A->num * B->den;
    __uint128_t rhs = (__uint128_t)B->num * A->den;
    if (lhs < rhs) return -1;
    if (lhs > rhs) return  1;
    return 0;
}

ull gcd(ull a, ull b){
    return b ? gcd(b, a % b) : a;
}

void insert_bike(int s, int x, ull pnum, ull pden, int output) {
    
    int sz = slot_size[x]++;
    slots[x][sz].s   = s;
    slots[x][sz].num = pnum;
    slots[x][sz].den = pden;
    student_slot[s] = x;
    
    ull g = gcd(pnum,pden);
    pnum /= g; pden /= g;
    if (!output) return;
    if (pden == 1ULL) {
        printf("%d parked at (%d, %llu).\n", s, x, pnum);
    } else {
        printf("%d parked at (%d, %llu/%llu).\n", s, x, pnum, pden);
    }
}

void park_bike(int s, int x, int p, int output) {
    // mark used integer spots
    int used[31] = {0};
    for (int i = 0; i < slot_size[x]; i++) {
        if (slots[x][i].den == 1) {
            int v = (int)slots[x][i].num;
            if (v >= 1 && v <= c[x]) used[v] = 1;
        }
    }
    // rule 1
    if (!used[p]) {
        insert_bike(s,x,p,1ULL,output);
        return;
    }
    // rule 2
    int best=-1, bestDiff=1e9;
    for (int i = 1; i <= c[x]; i++) {
        if (!used[i]) {
            int d = abs(i-p);
            if (d < bestDiff || (d==bestDiff && i<best)) {
                best=i; bestDiff=d;
        }}
    }
    if (best != -1) {
        insert_bike(s,x,best,1ULL,output);
        return;
    }
    // else fractional
    qsort(slots[x], slot_size[x], sizeof(struct pos), cmp_pos);
    // find exact p/1
    int idxp=-1;
    for (int i=0;i<slot_size[x];i++){
        if (slots[x][i].den==1 && slots[x][i].num==(ull)p){
            idxp=i; break;
        }
    }
    ull Pn=p, Pd=1;
    if (idxp>=0) {
        int nb = (idxp>0 ? idxp-1 : idxp+1);
        ull a = slots[x][nb].num, b = slots[x][nb].den;
        ull Nn = Pn*b + a, Nd = 2*b;
        insert_bike(s,x,Nn,Nd,output);
        return;
    }
    // else insert next to fractional position
    int idx=slot_size[x];
    for (int i=0;i<slot_size[x];i++){
        __uint128_t lhs = Pn*slots[x][i].den;
        __uint128_t rhs = slots[x][i].num*Pd;
        if (lhs < rhs) { idx=i; break; }
    }
    if (idx==0) {
        ull a=slots[x][0].num, b=slots[x][0].den;
        ull Nn = Pn*b + a, Nd = 2*b;
        insert_bike(s,x,Nn,Nd,output);
    } else {
        ull a=slots[x][idx-1].num, b=slots[x][idx-1].den;
        ull Nn = Pn*b + a, Nd = 2*b;
        insert_bike(s,x,Nn,Nd,output);
    }
}

int lca(int a, int b) {
    if (depth[a] < depth[b]){ 
        int t=a; a=b; b=t; 
    }
    for (int k = MAX_LOG-1; k >= 0; k--) {
        if (depth[ up[a][k] ] >= depth[b]) {
            a = up[a][k];
        }
    }
    if (a == b) return a;
    for (int k = MAX_LOG-1; k >= 0; k--) {
        if (up[a][k] != up[b][k]) {
            a = up[a][k];
            b = up[b][k];
        }
    }
    return up[a][0];
}

long long bit_sum(int i) {
    long long s = 0;
    for (; i > 0; i -= i & -i){
        s += bit[i];
    }
    return s;
}

long long currDist(int v) {
    return dist0[v] + bit_sum(tin[v]);
}

long long get_dist(int a, int b) {
    int c = lca(a,b);
    return currDist(a) + currDist(b) - 2*currDist(c);
}

void insert_notif(ull t, int s) {
    int pos = notif_count++;
    
    while (pos > 0 && all_notifs[pos-1].t > t) {
        all_notifs[pos] = all_notifs[pos-1];
        pos--;
    }
    all_notifs[pos].t = t;
    all_notifs[pos].s = s;
}

void do_fetch(ull T) {
    int cnt = 0;
    while (fetched_ptr < notif_count && all_notifs[fetched_ptr].t <= T) {
        fetched_ptr++;
        cnt++;
    }
    printf("At %llu, %d bicycles was fetched.\n", T, cnt);
}

void bit_update(int i, long long v) {
    for (; i <= N; i += i & -i){
        bit[i] += v;
    }
}