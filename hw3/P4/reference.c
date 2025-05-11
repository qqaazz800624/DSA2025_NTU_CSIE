/*  Device Symptom Aggregation –   fast rollback-DSU (800 ms OK)
   - no duplicate list, no virus-find compression
   - computer-find : union-by-size, **no path compression**            */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAXN 500005
#define NIL  (-1LL)

/* ---------- very fast integer input ---------- */
static inline int rd(void){
    int c, x = 0;
    while((c = getchar_unlocked()) < '0');
    do{ x = x*10 + (c & 15); c = getchar_unlocked(); }while(c >= '0');
    return x;
}

/* ---------- rollback stack ---------- */
typedef struct { long long *ptr, oldv; } Modify;
/* 650 萬格 ≈ 100 MB，已足夠最壞情況（實測 < 30 MB 用量） */
static Modify *stk;
static int     top = 0;
static int     opId = 0;          /* # reversible ops already done      */
static int    *hist;              /* hist[opId] = #MODs in that op      */

static inline void MOD(long long *p,long long v){
    stk[top++] = (Modify){p,*p};
    ++hist[opId];
    *p = v;
}
static inline void revertLast(void){
    for(int n = hist[--opId]; n--; ){
        Modify m = stk[--top];
        *m.ptr = m.oldv;
    }
}

/* ---------- tables ---------- */
/* computer DSU + network list */
static long long cPar[MAXN], cSize[MAXN], cDom[MAXN];
static long long nHead[MAXN], nTail[MAXN], nNxt[MAXN], nPrv[MAXN];
/* virus DSU + virus list */
static long long vPar[MAXN], vLvl[MAXN], vCnt[MAXN], vTag[MAXN];
static long long vHead[MAXN], vTail[MAXN], vNxt[MAXN], vPrv[MAXN];
/* per computer */
static long long baseDmg[MAXN], offset[MAXN], curVir[MAXN];

/* ---------- find ---------- */
static inline long long findC(long long x){      /* no compression */
    while(cPar[x]!=x) x=cPar[x];
    return x;
}
static inline long long findV(long long x){      /* keep virus list stable */
    while(vPar[x]!=x) x=vPar[x];
    return x;
}

/* ---------- list helpers ---------- */
static inline void detachNet(long long u,long long r){
    long long p=nPrv[u], n=nNxt[u];
    if(p!=NIL) MOD(&nNxt[p],n); else MOD(&nHead[r],n);
    if(n!=NIL) MOD(&nPrv[n],p); else MOD(&nTail[r],p);
    MOD(&nPrv[u],NIL); MOD(&nNxt[u],NIL);
}
static inline void appendVir(long long R,long long u){
    if(vHead[R]==NIL){
        MOD(&vHead[R],u); MOD(&vTail[R],u);
        MOD(&vPrv[u],NIL); MOD(&vNxt[u],NIL);
    }else{
        long long t=vTail[R];
        MOD(&vNxt[t],u);  MOD(&vPrv[u],t);
        MOD(&vNxt[u],NIL); MOD(&vTail[R],u);
    }
    MOD(&vCnt[R],vCnt[R]+1);
}
static inline void detachVir(long long u,long long R){
    long long p=vPrv[u], n=vNxt[u];
    if(p!=NIL) MOD(&vNxt[p],n); else MOD(&vHead[R],n);
    if(n!=NIL) MOD(&vPrv[n],p); else MOD(&vTail[R],p);
    MOD(&vPrv[u],NIL); MOD(&vNxt[u],NIL);
    MOD(&vCnt[R],vCnt[R]-1);
}
static inline void spliceVir(long long A,long long B){
    if(vHead[B]==NIL) return;
    if(vHead[A]==NIL){
        MOD(&vHead[A],vHead[B]); MOD(&vTail[A],vTail[B]);
    }else{
        long long t=vTail[A];
        MOD(&vNxt[t],vHead[B]); MOD(&vPrv[vHead[B]],t);
        MOD(&vTail[A],vTail[B]);
    }
    MOD(&vHead[B],NIL); MOD(&vTail[B],NIL);
}

/* ---------- operations ---------- */
static void opConnect(int x,int y){
    long long r1=findC(x), r2=findC(y); if(r1==r2) return;
    long long v1=findV(cDom[r1]), v2=findV(cDom[r2]);
    long long win = (vLvl[v1]>=vLvl[v2])? v1 : v2;

    /* union by size – keep r1 larger */
    if(cSize[r1] < cSize[r2]){ long long t=r1;r1=r2;r2=t; }

    /* merge DSU & list */
    MOD(&cPar[r2],r1);  MOD(&cSize[r1],cSize[r1]+cSize[r2]);
    if(nHead[r2]!=NIL){
        MOD(&nNxt[nTail[r1]],nHead[r2]);
        MOD(&nPrv[nHead[r2]],nTail[r1]);
        MOD(&nTail[r1],nTail[r2]);
        MOD(&nHead[r2],NIL); MOD(&nTail[r2],NIL);
    }
    MOD(&cDom[r1],win);

    /* change virus of all nodes (old ≠ win) */
    for(long long u=nHead[r1]; u!=NIL; u=nNxt[u]){
        long long old=findV(curVir[u]); if(old==win) continue;
        detachVir(u,old); appendVir(win,u);

        long long dmg = baseDmg[u] + (vTag[old]-offset[u]);
        MOD(&baseDmg[u],dmg); MOD(&offset[u],vTag[win]); MOD(&curVir[u],win);
    }
}

static inline void opEvolve (int t){ long long r=findV(t); MOD(&vLvl[r],vLvl[r]+1); }
static inline void opAttack (int t){ long long r=findV(t); MOD(&vTag[r],vTag[r]+vLvl[r]); }

static void opFusion(int a,int b){
    long long r1=findV(a), r2=findV(b); if(r1==r2) return;
    if(vCnt[r1] < vCnt[r2]){ long long t=r1;r1=r2;r2=t; }

    MOD(&vPar[r2],r1); MOD(&vLvl[r1],vLvl[r1]+vLvl[r2]);
    MOD(&vCnt[r1],vCnt[r1]+vCnt[r2]); MOD(&vCnt[r2],0);

    for(long long u=vHead[r2]; u!=NIL; u=vNxt[u]){
        long long dmg=baseDmg[u] + (vTag[r2]-offset[u]);
        MOD(&baseDmg[u],dmg); MOD(&offset[u],vTag[r1]); MOD(&curVir[u],r1);
    }
    spliceVir(r1,r2); MOD(&vTag[r2],vTag[r1]);
}

static void opReinstall(int k,int s){
    long long netR=findC(k), oldV=findV(curVir[k]), newV=findV(s);
    detachNet(k,netR); MOD(&cSize[netR],cSize[netR]-1);

    if(netR==k && cSize[k]>0){              /* promote next node */
        long long nr=nHead[k];
        MOD(&cPar[nr],nr); MOD(&cSize[nr],cSize[k]);
        MOD(&cDom[nr],cDom[k]);
        MOD(&nHead[nr],nHead[k]); MOD(&nTail[nr],nTail[k]);
        for(long long u=nHead[nr];u!=NIL;u=nNxt[u]) if(u!=nr) MOD(&cPar[u],nr);
        MOD(&cSize[k],0); MOD(&nHead[k],NIL); MOD(&nTail[k],NIL);
    }
    MOD(&cPar[k],k); MOD(&cSize[k],1); MOD(&cDom[k],newV);
    MOD(&nHead[k],k); MOD(&nTail[k],k);

    if(oldV!=newV){ detachVir(k,oldV); appendVir(newV,k); }
    MOD(&baseDmg[k],0); MOD(&offset[k],vTag[newV]); MOD(&curVir[k],newV);
}

static inline void opStatus(int k){
    long long r=findV(curVir[k]);
    long long dmg=baseDmg[k] + (vTag[r]-offset[k]);
    printf("%lld %lld %lld\n", dmg, vLvl[r], vCnt[r]);
}

/* ---------- main ---------- */
int main(void){
    /* 預先一次配置足夠的 rollback stack & hist */
    stk  = (Modify*)malloc(sizeof(Modify)*6500000);
    hist = (int*)   calloc(MAXN, sizeof(int));

    int N = rd(), Q = rd();

    for(int i=1;i<=N;i++){
        cPar[i]=i; cSize[i]=1; cDom[i]=i;
        nHead[i]=nTail[i]=i; nNxt[i]=nPrv[i]=NIL;

        vPar[i]=i; vLvl[i]=1; vCnt[i]=1; vTag[i]=0;
        vHead[i]=vTail[i]=i; vNxt[i]=vPrv[i]=NIL;

        baseDmg[i]=0; offset[i]=0; curVir[i]=i;
    }

    while(Q--){
        int tp = rd();
        if(tp!=6 && tp!=7) hist[opId]=0;

        switch(tp){
            case 1:{ int x=rd(),y=rd(); opConnect(x,y); ++opId; } break;
            case 2:{ opEvolve(rd()); ++opId; } break;
            case 3:{ opAttack(rd()); ++opId; } break;
            case 4:{ int k=rd(),s=rd(); opReinstall(k,s); ++opId; } break;
            case 5:{ int a=rd(),b=rd(); opFusion(a,b); ++opId; } break;
            case 6:{ opStatus(rd()); } break;
            case 7:{ revertLast(); } break;
            default: assert(0);
        }
    }
    return 0;
}
