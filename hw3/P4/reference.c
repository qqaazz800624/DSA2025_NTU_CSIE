#include <stdio.h>
#include <stdlib.h>

#define MAX_N 500000
#define MAX_Q 500000

typedef struct {
    long long *ptr;
    long long original_value;
    } Modify;

typedef struct {
    Modify *data;
    int size, capacity;
} ModifyStack;

ModifyStack st;
int op_count = 0;
int hist_size[MAX_Q]; 

static long long vp          [MAX_N + 1];
static long long v_rank      [MAX_N + 1];
static long long v_setsize   [MAX_N + 1];  
static long long v_level     [MAX_N + 1];  
static long long cumAtk      [MAX_N + 1];  
static long long vc          [MAX_N + 1]; 

long long cp               [MAX_N + 1];    
long long c_rank           [MAX_N + 1];    
long long c_rootVirus      [MAX_N + 1];    
long long c_child          [MAX_N + 1];    
long long c_leftSib        [MAX_N + 1];    
long long c_rightSib       [MAX_N + 1];    
long long c_snap           [MAX_N + 1];    
long long c_netsize        [MAX_N + 1];    
long long c_shift          [MAX_N + 1];

typedef struct pair {
    long long z;
    long long p;
} Pair;

void initModifyStack(void);
void modify(long long *ptr, long long value);
void undoLastOp();
void undoOps(int k);
void assigncp(long long x, long long newParent);
Pair findv(long long x);
Pair findc(long long x);
long long calAtk(long long v);
long long uv(long long x, long long y);
long long uc(long long x, long long y);
void resetvalues(long long x);
void rmv(long long x);
void evolve(long long v);
void attack(long long v);
void reinstall(long long x, long long v);
void status(long long x);
void ms(long long i);

int main(int argc, char *argv[]) {
    initModifyStack();
    if (argc >= 2) {
        if (!freopen(argv[1], "r", stdin)) {
            perror("freopen input");
            return 1;
        }
    }
    if (argc >= 3) {
        if (!freopen(argv[2], "w", stdout)) {
            perror("freopen output");
            return 1;
        }
    }

    int n, q;
    if (scanf("%d%d", &n, &q) != 2) {
        fprintf(stderr, "failed to read n and q\n");
        return 1;
    }
    for (int i = 1; i <= n; i++) {
        ms(i);
    }
    int op;
    for (int i = 0; i < q; i++) {
        if (argc==2) printf("op");
        scanf("%d", &op);
        switch (op) {
            case 1: {
                int x, y;
                scanf("%d%d", &x, &y);
                if (argc == 2) printf("%d %d %d\n", 1, x, y);
                hist_size[op_count] = 0;
                uc(x, y);
                op_count++;
                break;
            }
            case 2: {
                int t;
                scanf("%d", &t);
                if (argc == 2) printf("%d %d\n", 2, t);
                hist_size[op_count] = 0;
                evolve(t);
                op_count++;
                break;
            }
            case 3: {
                int t;
                scanf("%d", &t);
                if (argc == 2) printf("%d %d\n", 3, t);
                hist_size[op_count] = 0;
                attack(t);
                op_count++;
                break;
            }
            case 4: {
                int k, s;
                scanf("%d%d", &k, &s);
                if (argc == 2) printf("%d %d %d\n", 4, k, s);
                hist_size[op_count] = 0;
                reinstall(k, s);
                op_count++;
                break;
            }
            case 5: {
                int a, b;
                scanf("%d%d", &a, &b);
                if (argc == 2) printf("%d %d %d\n", 5, a, b);
                hist_size[op_count] = 0;
                uv(a, b);
                op_count++;
                break;
            }
            case 6: {
                int k;
                scanf("%d", &k);
                if (argc == 2) printf("%d %d\n", 6, k);
                status(k);
                break;
            }
            case 7: {
                if (argc == 2) printf("7");
                undoLastOp();
                break;
            }
        }
    }
    return 0;
}


void initModifyStack(void) {
    st.size     = 0;
    st.capacity = 1;                
    st.data     = malloc(sizeof(Modify) * st.capacity);
    if (!st.data) { perror("malloc"); exit(1); }
}

void modify(long long *ptr, long long value) {
    if (*ptr == value) {
        return;
    }
    if (st.size == st.capacity) {
        st.capacity *= 2;
        st.data = (Modify *)realloc(st.data, sizeof(Modify) * st.capacity);
    }
    st.data[st.size].ptr = ptr;
    st.data[st.size].original_value = *ptr;
    st.size++;
    hist_size[op_count]++;
    *ptr = value;
}

void undoLastOp() {
    if (op_count == 0) return;           
    int mods = hist_size[--op_count];
    while (mods-- > 0) {
        Modify m = st.data[--st.size];
        *m.ptr   = m.original_value;
    }
}

void undoOps(int k) {
    while (k-- > 0 && op_count > 0) {
        undoLastOp();
    }
}

void assigncp(long long x, long long newParent) {
    long long l = c_leftSib[x];
    long long r = c_rightSib[x];

    if (c_child[cp[x]] == x) {
        modify(&c_child[cp[x]], r); 
    }
    if (l >= 0) {
        modify(&c_rightSib[l], r);
    }
    if (r >= 0){
        modify(&c_leftSib[r], l);
    }

    if (cp[x] == x && vc[c_rootVirus[x]] == x) {
        modify(&vc[c_rootVirus[x]], r); 
    }

    long long c = c_child[newParent];
    if (c >= 0){
        modify(&c_leftSib[c], x); 
    }
    modify(&c_child[newParent], x); 
    modify(&c_rightSib[x], c); 
    modify(&c_leftSib[x], 0); 
    modify(&cp[x], newParent); 
}


Pair findv(long long x) {
    if (vp[x] == x) {
        return (Pair){ .p = x, .z = 0 };
    }
    Pair pr = findv(vp[x]);
    modify(&vp[x], pr.p);
    long long total = cumAtk[x] + pr.z;
    modify(&cumAtk[x], total);
    return (Pair){ .p = pr.p, .z = total };
}

Pair findc(long long x) {
    if (cp[x] == x) {
        return (Pair){ .p = x, .z = 0 };
    }
    Pair pr = findc(cp[x]);
    assigncp(x, pr.p);
    long long total = c_shift[x] + pr.z;
    modify(&c_shift[x], total);
    return (Pair){ .p = pr.p, .z = total };
}

long long calAtk(long long v) {
    long long atk = cumAtk[v];
    while (vp[v] != v) {
        v = vp[v];
        atk += cumAtk[v];
    }
    return atk;
}

long long uv(long long x, long long y) {
    Pair px = findv(x);
    long long rx = px.p;
    long long sx = px.z;

    Pair py = findv(y);
    long long ry = py.p;
    long long sy = py.z;

    if (rx == ry) return rx;

    long long r;
    long long c;

    if (v_rank[rx] > v_rank[ry]) {
        r = rx;
        c = ry;
    } else {
        r = ry;
        c = rx;
        if (v_rank[rx] == v_rank[ry]) modify(&v_rank[ry], v_rank[ry] + 1);
    }

    modify(&vp[c], r);
    modify(&v_setsize[r], v_setsize[r] + v_setsize[c]);
    modify(&v_level[r], v_level[r] + v_level[c]); 
    modify(&cumAtk[c], cumAtk[c] - cumAtk[r]);
    return r;
}


long long uc(long long x, long long y) {
    long long rx = findc(x).p;
    long long ry = findc(y).p;
    if (rx == ry) return rx;

    long long vx = c_rootVirus[rx];
    long long vy = c_rootVirus[ry];
    long long v;
    if (v_level[findv(vx).p] >= v_level[findv(vy).p]) v = vx;
    else v = vy;

    long long ax = calAtk(vx);
    long long ay = calAtk(vy);

    modify(&c_shift[rx], c_shift[rx] + ax - c_snap[rx]);
    modify(&c_snap[rx], ax);
    modify(&c_shift[ry], c_shift[ry] + ay - c_snap[ry]);
    modify(&c_snap[ry], ay);

    long long r;
    long long c;
    long long rv;
    long long cv;
    long long ra;
    long long ca;
    if (c_rank[rx] > c_rank[ry]) {
        r = rx;
        rv = vx;
        ra = ax;
        c = ry;
        cv = vy;
        ca = ay;
    } else {
        r = ry;
        rv = vy;
        ra = ay;
        c = rx;
        cv = vx;
        ca = ax;
        if (c_rank[rx] == c_rank[ry]) modify(&c_rank[ry], c_rank[ry] + 1);
    }

    long long cleft = c_leftSib[c];
    long long cright = c_rightSib[c];
    if (cleft != 0) modify(&c_rightSib[cleft],cright);
    if (cright != 0) modify(&c_leftSib[cright], cleft);
    if (vc[cv] == c) modify(&vc[cv], cright);

    modify(&cp[c], r);
    modify(&c_leftSib[c], 0);
    modify(&c_rightSib[c], c_child[r]);
    if (c_child[r] != 0) modify(&c_leftSib[c_child[r]], c);
    modify(&c_child[r], c);
    
    if (v != rv) {
        long long rleft = c_leftSib[r];
        long long rright = c_rightSib[r];
        if (rleft != 0) modify(&c_rightSib[rleft], rright);
        if (rright != 0) modify(&c_leftSib[rright], rleft);
        if (vc[rv] == r) modify(&vc[rv], rright);

        modify(&c_rightSib[r], vc[v]);
        if (vc[v] != 0) modify(&c_leftSib[vc[v]], r);
        modify(&c_leftSib[r], 0);
        modify(&vc[v], r);
        modify(&c_snap[r], calAtk(v));
    }

    modify(&c_shift[c], c_shift[c] - c_shift[r]);

    modify(&c_rootVirus[r], v);
    
    modify(&v_setsize[findv(rv).p], v_setsize[findv(rv).p] - c_netsize[r]);
    modify(&v_setsize[findv(cv).p], v_setsize[findv(cv).p] - c_netsize[c]);
    modify(&c_netsize[r], c_netsize[r] + c_netsize[c]);
    modify(&v_setsize[findv(v).p], v_setsize[findv(v).p] + c_netsize[r]);

    return r;
}

void resetvalues(long long x) {
    modify(&c_netsize[x], 1); 
    modify(&cp[x], x);
    modify(&c_child[x], 0);
    modify(&c_leftSib[x], 0);
    modify(&c_rightSib[x], 0);
    modify(&c_rootVirus[x], 0);
    modify(&c_rank[x], 0);
    modify(&c_shift[x], 0);
}

void rmv(long long x) {

    if (cp[x] == x) { 
        long long xs = c_shift[x];
        long long v = c_rootVirus[x];
        long long xl = c_leftSib[x];
        long long xr = c_rightSib[x];
        long long newRoot = c_child[x];

        if (newRoot == 0) {
            if (vc[v] == x) modify(&vc[v], xr); 
            if (xl != 0)
                modify(&c_rightSib[xl], xr); 
            if (xr != 0)
                modify(&c_leftSib[xr], xl) ; 
            long long rv = findv(v).p;
            modify(&v_setsize[rv], v_setsize[rv] - 1); 

            resetvalues(x);
            return;
        }

        long long newRootOldShift = c_shift[newRoot];
        long long c = c_child[newRoot];
        long long nr_rsib = c_rightSib[newRoot]; 
        
        if (c == 0) { 
            modify(&c_child[newRoot], nr_rsib); 
        } else {
            while (c_rightSib[c] != 0)
                c = c_rightSib[c];
            modify(&c_rightSib[c], nr_rsib); 
        }
        if (nr_rsib != 0) 
            modify(&c_leftSib[nr_rsib] , c); 
        while (nr_rsib != 0) {
            modify(&cp[nr_rsib], newRoot); 
            modify(&c_shift[nr_rsib], c_shift[nr_rsib] - newRootOldShift); 
            nr_rsib = c_rightSib[nr_rsib];
        }

        modify(&c_shift[newRoot], c_shift[newRoot] + xs); 
        modify(&c_rootVirus[newRoot], v); 
        modify(&c_netsize[newRoot], c_netsize[x] - 1); 
        modify(&cp[newRoot], newRoot);
        modify(&c_rank[newRoot], c_rank[x] - 1);
        modify(&c_snap[newRoot], c_snap[x]); 
        long long rv = findv(v).p;
        modify(&v_setsize[rv], v_setsize[rv] - 1);

        if (vc[v] == x) modify(&vc[v], newRoot);
        modify(&c_leftSib[newRoot], xl); 
        if (xl != 0)
            modify(&c_rightSib[xl], newRoot); 
        modify(&c_rightSib[newRoot], xr); 
        if (xr != 0)
            modify(&c_leftSib[xr], newRoot); 

    } else { 
        Pair pair = findc(x);
        while (c_child[x] != 0) {
            findc(c_child[x]);
        }
        int xl = c_leftSib[x];
        int xr = c_rightSib[x];
        if (xl != 0)
            modify(&c_rightSib[xl], xr); 
        if (xr != 0)
            modify(&c_leftSib[xr], xl); 

        if (c_child[pair.p] == x) modify(&c_child[pair.p], xr); 
        long long v = findv(c_rootVirus[pair.p]).p;
        modify(&v_setsize[v], v_setsize[v] - 1);
        modify(&c_netsize[pair.p], c_netsize[pair.p] - 1); 
    }   
    resetvalues(x);
}

void evolve(long long v) {
    v = findv(v).p;
    modify(&v_level[v], v_level[v] + 1); 
}

void attack(long long v) {
    v = findv(v).p;
    modify(&cumAtk[v], cumAtk[v] + v_level[v]); 
}

void reinstall(long long x, long long v) {
    rmv(x); 
    v = findv(v).p;
    modify(&c_snap[x], calAtk(v)); 
    modify(&c_rightSib[x], vc[v]); 
    if (vc[v] != 0)
        modify(&c_leftSib[vc[v]], x); 
    modify(&vc[v], x); 
    modify(&c_rootVirus[x], v);
    modify(&v_setsize[v], v_setsize[v] + 1); 
}

void status(long long x) {
    long long d = 0;
    while (cp[x] != x) {
        d += c_shift[x];
        x = cp[x];
    }
    
    d += c_shift[x] - c_snap[x];
    long long v = c_rootVirus[x];
    d += cumAtk[v];
    while (vp[v] != v) {
        v = vp[v];
        d += cumAtk[v];
    }
    long long n_infected = v_setsize[v];
    printf("%lld %lld %lld\n", d, v_level[v], n_infected);

}

void ms(long long i) {
    vp[i] = i;
    v_level[i] = 1;
    cumAtk[i] = 0;

    cp[i] = i;
    c_leftSib[i] = 0;
    c_rightSib[i] = 0;
    c_child[i] = 0;
    c_snap[i] = 0;
    c_shift[i] = 0;
    c_rank[i] = 0;
    c_netsize[i] = 1;

    vc[i] = i;
    c_rootVirus[i] = i;
    v_setsize[i] = 1;

}