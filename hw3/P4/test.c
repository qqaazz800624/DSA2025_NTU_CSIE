#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAXN 500005

/* ---------- fast input ---------- */
static inline int rd(void){
    int c, x = 0;
    while((c = getchar_unlocked()) < '0');
    do{ x = x * 10 + (c & 15); c = getchar_unlocked(); } while(c >= '0');
    return x;
}

/* ---------- rollback stack ---------- */
typedef struct { long long *ptr, oldv; } Modify;
static Modify *stk;
static int top = 0, cap = 1 << 20;
static int opId = 0, hist[MAXN + 5];

static inline void ensure(void) {
    if (top == cap) {
        cap <<= 1;
        stk = (Modify*)realloc(stk, sizeof(Modify) * cap);
        assert(stk);
    }
}
#define MOD(P, VAL) ( ensure(), stk[top++] = (Modify){(long long*)(P), *(P)}, ++hist[opId], *(P) = (VAL) )
static inline void revertLast(void) {
    if (!opId) return;
    for (int n = hist[--opId]; n--;) {
        Modify m = stk[--top];
        *m.ptr = m.oldv;
    }
}

/* ---------- global arrays ---------- */
static int cp[MAXN], csz[MAXN], cVirus[MAXN];
static long long shift_[MAXN], snap[MAXN];

static int vp[MAXN], vLvl[MAXN], vCnt[MAXN];
static long long vEdge[MAXN];  // edge weight to parent in virus tree

/* ---------- helpers ---------- */
static int findC(int x) {
    if (cp[x] == x) return x;
    int p = cp[x];
    int r = findC(p);
    if (cp[x] != r) MOD(&cp[x], r);
    MOD(&shift_[x], shift_[x] + shift_[p]);
    return r;
}

static int findV(int x) {
    if (vp[x] == x) return x;
    int p = vp[x];
    int r = findV(p);
    MOD(&vEdge[x], vEdge[x] + vEdge[p]);
    MOD(&vp[x], r);
    return r;
}

static long long tagOf(int v) {
    long long acc = vEdge[v];
    while (vp[v] != v) {
        v = vp[v];
        acc += vEdge[v];
    }
    return acc;
}

/* ---------- operations ---------- */
static void opConnect(int x, int y) {
    int rx = findC(x), ry = findC(y);
    if (rx == ry) return;

    int vx = findV(cVirus[rx]);
    int vy = findV(cVirus[ry]);

    if (csz[rx] < csz[ry]) {
        int tmp = rx; rx = ry; ry = tmp;
        tmp = vx; vx = vy; vy = tmp;
    }

    int win  = (vLvl[vx] >= vLvl[vy]) ? vx : vy;
    int lose = (win == vx) ? vy : vx;

    long long tagRxOld = tagOf(vx);
    long long tagRyOld = tagOf(vy);
    long long tagWin   = tagOf(win);
    long long tagLose  = tagOf(lose);  // ⚠️ Needed to compute ry shift correctly

    MOD(&cp[ry], rx);
    MOD(&csz[rx], csz[rx] + csz[ry]);

    MOD(&snap[rx], snap[rx] + (tagWin - tagRxOld));
    MOD(&shift_[ry], shift_[ry] + (tagLose - tagWin));
    MOD(&snap[ry], snap[rx]);

    MOD(&cVirus[rx], win);
    MOD(&vCnt[win], vCnt[win] + vCnt[lose]);
    MOD(&vCnt[lose], 0);
}

static void opEvolve(int t) {
    int r = findV(t);
    MOD(&vLvl[r], vLvl[r] + 1);
}

static void opAttack(int c) {
    int root = findC(c);
    int v = findV(cVirus[root]);
    MOD(&vEdge[v], vEdge[v] + vLvl[v]);
}

static void opFusion(int a, int b) {
    int ra = findV(a), rb = findV(b);
    if (ra == rb) return;
    if (vCnt[ra] < vCnt[rb]) { int t = ra; ra = rb; rb = t; }

    long long tagRa = tagOf(ra);
    long long tagRb = tagOf(rb);

    MOD(&vp[rb], ra);
    MOD(&vEdge[rb], tagRb - tagRa);

    MOD(&vLvl[ra], vLvl[ra] + vLvl[rb]);
    MOD(&vCnt[ra], vCnt[ra] + vCnt[rb]);
    MOD(&vCnt[rb], 0);
}

static void opReinstall(int k, int s) {
    int rk = findC(k);
    int oldV = findV(cVirus[rk]), newV = findV(s);

    long long acc = 0, cur = k;
    while (cur != rk) { acc += shift_[cur]; cur = cp[cur]; }
    long long dmg = acc + shift_[rk] + tagOf(oldV) - snap[rk];
    MOD(&shift_[k], dmg);

    MOD(&cp[k], k);
    MOD(&shift_[k], 0);
    MOD(&csz[rk], csz[rk] - 1);

    MOD(&vCnt[oldV], vCnt[oldV] - 1);
    MOD(&vCnt[newV], vCnt[newV] + 1);

    MOD(&cVirus[k], newV);
    MOD(&snap[k], tagOf(newV));
}

static void opStatus(int k) {
    int x = k; long long acc = 0;
    while (cp[x] != x) { acc += shift_[x]; x = cp[x]; }
    int root = x;
    int v = findV(cVirus[root]);
    long long dmg = acc + shift_[root] + tagOf(v) - snap[root];
    printf("%lld %d %d\n", dmg, vLvl[v], vCnt[v]);
}

/* ---------- main ---------- */
int main(void) {
    stk = (Modify*)malloc(sizeof(Modify) * cap);

    int N = rd(), Q = rd();
    for (int i = 1; i <= N; i++) {
        cp[i] = i; csz[i] = 1; cVirus[i] = i; shift_[i] = 0; snap[i] = 0;
        vp[i] = i; vLvl[i] = 1; vEdge[i] = 0; vCnt[i] = 1;
    }

    while (Q--) {
        int tp = rd();
        if (tp != 6 && tp != 7) hist[opId] = 0;

        switch (tp) {
            case 1: { int x = rd(), y = rd(); opConnect(x, y); ++opId; } break;
            case 2: { int t = rd(); opEvolve(t); ++opId; } break;
            case 3: { int c = rd(); opAttack(c); ++opId; } break;
            case 4: { int k = rd(), s = rd(); opReinstall(k, s); ++opId; } break;
            case 5: { int a = rd(), b = rd(); opFusion(a, b); ++opId; } break;
            case 6: { int k = rd(); opStatus(k); } break;
            case 7: { revertLast(); } break;
            default: assert(0);
        }
    }
    return 0;
}
