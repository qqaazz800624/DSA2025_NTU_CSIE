#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAXN 500005
#define NIL   (-1LL)

/* ---------- fast input ---------- */
static inline int rd(void){
    int c, x = 0;
    while((c = getchar_unlocked()) < '0');
    do{ x = x*10 + (c&15); c=getchar_unlocked(); }while(c>='0');
    return x;
}

/* ---------- rollback stack ---------- */
typedef struct{ long long *ptr, oldv; } Modify;
static Modify *stk;            static int top=0, cap=1<<20;
static int opId=0;             static int hist[MAXN+5];

static inline void ensure(void){
    if(top==cap){
        cap <<= 1;
        stk = (Modify*)realloc(stk,sizeof*stk*cap);
        assert(stk);
    }
}
#define MOD(P,VAL)  ( ensure(), stk[top++] = (Modify){(long long*)(P), *(P)}, \
                      ++hist[opId], *(P) = (VAL) )

static inline void revertLast(void){
    if(!opId) return;
    for(int n=hist[--opId]; n--; ){
        Modify m = stk[--top];
        *m.ptr = m.oldv;
    }
}

/* ---------- arrays ---------- */
static int    cp[MAXN], csz[MAXN], cVirus[MAXN];
static long long shift_[MAXN], snap[MAXN];

static int    vp[MAXN], vLvl[MAXN], vCnt[MAXN];
static long long vTag[MAXN];

/* ---------- find (computer) ---------- */
static int findC(int x){
    if(cp[x]==x) return x;
    int p = cp[x];
    int r = findC(p);
    if(cp[x]!=r) MOD(&cp[x], r);            /* path-halving */
    MOD(&shift_[x], shift_[x] + shift_[p]); /* accumulate edge weight */
    return r;
}
/* ---------- find (virus, no compression) ---------- */
static int findV(int x){ while(vp[x]!=x) x = vp[x]; return x; }


/* ---------- operations ---------- */
static void opConnect(int x,int y)
{
    int rx = findC(x);          /* current roots of the two networks   */
    int ry = findC(y);
    if (rx == ry){              /* already the same network – undo-able */
        return;                 /* (main() will still ++opId afterwards)*/
    }

    /* viruses that currently dominate the two roots                    */
    int vx = findV(cVirus[rx]);
    int vy = findV(cVirus[ry]);

    /* -------- size heuristic: always attach the smaller tree -------- */
    if (csz[rx] < csz[ry]){          /* swap roots **and their viruses** */
        int tmp = rx;  rx = ry;  ry = tmp;
        tmp    = vx;  vx = vy;  vy = tmp;
    }

    /* -------- decide which virus wins (rule uses x-side on tie) ----- */
    int win  = (vLvl[vx] >= vLvl[vy]) ? vx : vy;
    int lose = (win == vx)            ? vy : vx;

    /* -------- union the two trees ----------------------------------- */
    MOD(&cp[ry] , rx);                       /* parent pointer          */
    MOD(&csz[rx], csz[rx] + csz[ry]);        /* subtree size            */

    /* -------- keep all existing damage numbers unchanged ------------ */
    long long tagRxOld = vTag[vx];           /* tag before the merge    */
    long long tagRyOld = vTag[vy];
    long long tagWin   = vTag[win];

    /* 1. if root-rx changes its virus, adjust its snap                 */
    MOD(&snap[rx], snap[rx] + (tagWin - tagRxOld));

    /* 2. set the edge weight ry→rx so every node below ry
          continues to see the same damage                               */
    long long newEdge = (tagRyOld - tagWin) + (snap[rx] - snap[ry]);
    MOD(&shift_[ry], shift_[ry] + newEdge);

    /* 3. align ry.snap to the new root’s snap                          */
    MOD(&snap[ry], snap[rx]);

    /* 4. bookkeeping for the dominant virus                            */
    MOD(&cVirus[rx], win);
    MOD(&vCnt[win] , vCnt[win] + vCnt[lose]);
    MOD(&vCnt[lose], 0);
}


static inline void opEvolve(int t){ int r=findV(t); MOD(&vLvl[r], vLvl[r]+1); }
static inline void opAttack(int c){                 /* NOTE: c is *computer* id */
    int root = findC(c); int v=findV(cVirus[root]);
    MOD(&vTag[v], vTag[v]+vLvl[v]);
}

static void opFusion(int a,int b){
    int ra=findV(a), rb=findV(b); if(ra==rb) return;
    if(vCnt[ra] < vCnt[rb]){ int t=ra; ra=rb; rb=t; }

    MOD(&vp[rb], ra);
    MOD(&vLvl[ra], vLvl[ra]+vLvl[rb]);
    MOD(&vTag[ra], vTag[ra]+vTag[rb]);
    MOD(&vCnt[ra], vCnt[ra]+vCnt[rb]);
    MOD(&vCnt[rb], 0);
}

static void opReinstall(int k,int s){
    int rk=findC(k);
    int oldV=findV(cVirus[rk]), newV=findV(s);

    /* flush personal pending damage into shift_[k] */
    long long acc=0, cur=k;
    while(cur!=rk){ acc+=shift_[cur]; cur=cp[cur]; }
    long long dmg = acc + shift_[rk] + vTag[oldV] - snap[rk];
    MOD(&shift_[k], dmg);

    /* detach k as a singleton */
    MOD(&cp[k], k); MOD(&shift_[k], 0);
    MOD(&csz[rk], csz[rk]-1);

    /* virus population update */
    MOD(&vCnt[oldV], vCnt[oldV]-1);
    MOD(&vCnt[newV], vCnt[newV]+1);

    MOD(&cVirus[k], newV);
    MOD(&snap[k]  , vTag[newV]);
}

static void opStatus(int k){
    int x=k; long long acc=0;
    while(cp[x]!=x){ acc+=shift_[x]; x=cp[x]; }
    int root=x, v=findV(cVirus[root]);
    long long dmg = acc + shift_[root] + vTag[v] - snap[root];
    printf("%lld %d %d\n", dmg, vLvl[v], vCnt[v]);
}

/* ---------- main ---------- */
int main(void){
    stk = (Modify*)malloc(sizeof(Modify)*cap);

    int N=rd(), Q=rd();
    for(int i=1;i<=N;i++){
        cp[i]=i; csz[i]=1; cVirus[i]=i; shift_[i]=0; snap[i]=0;
        vp[i]=i; vLvl[i]=1; vTag[i]=0; vCnt[i]=1;
    }

    while(Q--){
        int tp = rd();
        if(tp!=6 && tp!=7) hist[opId]=0;   /* open a fresh slot */

        switch(tp){
        case 1:{ int x=rd(),y=rd(); opConnect(x,y); ++opId; } break;
        case 2:{ int t=rd();        opEvolve(t);   ++opId; } break;
        case 3:{ int c=rd();        opAttack(c);   ++opId; } break;
        case 4:{ int k=rd(),s=rd(); opReinstall(k,s); ++opId;} break;
        case 5:{ int a=rd(),b=rd(); opFusion(a,b); ++opId; } break;
        case 6:{ int k=rd();        opStatus(k);         } break;
        case 7:{ revertLast();                           } break;
        default: assert(0);
        }
    }
    return 0;
}