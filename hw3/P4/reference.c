/*  Device Symptom Aggregation – rollback-DSU implementation  */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAXN 500005
#define MAXQ 500005
#define NIL  (-1LL)

/* ---------- rollback stack ------------------------------------- */
typedef struct { long long *ptr, oldv; } Modify;
static Modify *stk;
static int     top = 0, cap = 1 << 20;
static int     opId = 0;              /* completed reversible ops   */
static int     hist[MAXQ + 5];        /* modifications per op       */

static void ensure(void){
    if(top == cap){
        cap <<= 1;
        stk  = realloc(stk, sizeof *stk * cap);
        if(!stk){ perror("realloc"); exit(1); }
    }
}
static void MOD(long long *p, long long v){
    ensure();
    stk[top++] = (Modify){p,*p};
    ++hist[opId];
    *p = v;
}
static void revertLast(void){
    if(!opId) return;
    for(int n = hist[--opId]; n--; ){
        Modify m = stk[--top];
        *m.ptr   = m.oldv;
    }
}

/* ---------- global arrays -------------------------------------- */
/* computer-side DSU + net list */
static long long cPar[MAXN], cSize[MAXN], cDomVirus[MAXN];
static long long netHead[MAXN], netTail[MAXN], nxtNet[MAXN], prvNet[MAXN];

/* virus-side DSU + virus list */
static long long vPar[MAXN], vLvl[MAXN], vCnt[MAXN], vTag[MAXN];
static long long virHead[MAXN], virTail[MAXN], nxtVir[MAXN], prvVir[MAXN];

/* per computer */
static long long baseDmg[MAXN], offset[MAXN], curVir[MAXN];

/* DSU find (no path compression → rollback-safe) */
static long long findComp(long long x){ while(cPar[x]!=x) x=cPar[x]; return x; }
static long long findVir (long long x){ while(vPar[x]!=x) x=vPar[x]; return x; }

/* ---------- network list helpers ------------------------------- */
static void detachNet(long long u,long long root){
    long long p = prvNet[u], n = nxtNet[u];
    if(p!=NIL) MOD(&nxtNet[p], n); else MOD(&netHead[root], n);
    if(n!=NIL) MOD(&prvNet[n], p); else MOD(&netTail[root], p);
    MOD(&prvNet[u],NIL); MOD(&nxtNet[u],NIL);
}

/* ---------- virus list helpers --------------------------------- */
static void appendVir(long long R,long long u){
    if(virHead[R]==NIL){
        MOD(&virHead[R],u); MOD(&virTail[R],u);
        MOD(&prvVir[u],NIL); MOD(&nxtVir[u],NIL);
    }else{
        long long t = virTail[R];
        MOD(&nxtVir[t],u);
        MOD(&prvVir[u],t);
        MOD(&nxtVir[u],NIL);
        MOD(&virTail[R],u);
    }
    MOD(&vCnt[R], vCnt[R]+1);
}

static void detachVir(long long u,long long R){
    long long p = prvVir[u], n = nxtVir[u];
    if(p!=NIL) MOD(&nxtVir[p], n); else MOD(&virHead[R], n);
    if(n!=NIL) MOD(&prvVir[n], p); else MOD(&virTail[R], p);
    MOD(&prvVir[u],NIL); MOD(&nxtVir[u],NIL);
    MOD(&vCnt[R], vCnt[R]-1);
}
static void spliceVirusList(long long A,long long B){
    if(virHead[B]==NIL) return;
    if(virHead[A]==NIL){
        MOD(&virHead[A],virHead[B]);
        MOD(&virTail[A],virTail[B]);
    }else{
        long long t = virTail[A];
        MOD(&nxtVir[t],virHead[B]);
        MOD(&prvVir[virHead[B]],t);
        MOD(&virTail[A],virTail[B]);
    }
    MOD(&virHead[B],NIL); MOD(&virTail[B],NIL);
}

/* ---------- operations ----------------------------------------- */
static void opConnect(int x,int y){
    long long r1 = findComp(x), r2 = findComp(y);
    if(r1==r2) return;

    long long v1 = findVir(cDomVirus[r1]);
    long long v2 = findVir(cDomVirus[r2]);
    long long win = (vLvl[v1]>=vLvl[v2])? v1 : v2;

    if(cSize[r1] < cSize[r2]){ long long t=r1; r1=r2; r2=t; }
    MOD(&cPar[r2],r1);
    MOD(&cSize[r1],cSize[r1]+cSize[r2]);

    /* splice network list */
    if(netHead[r2]!=NIL){
        MOD(&nxtNet[netTail[r1]],netHead[r2]);
        MOD(&prvNet[netHead[r2]],netTail[r1]);
        MOD(&netTail[r1],netTail[r2]);
        MOD(&netHead[r2],NIL); MOD(&netTail[r2],NIL);
    }
    MOD(&cDomVirus[r1],win);

    for(long long u=netHead[r1]; u!=NIL; u=nxtNet[u]){
        long long old = findVir(curVir[u]);
        if(old==win) continue;
        detachVir(u,old);  appendVir(win,u);
        long long dmg = baseDmg[u] + (vTag[old]-offset[u]);
        MOD(&baseDmg[u],dmg); MOD(&offset[u],vTag[win]); MOD(&curVir[u],win);
    }
}

static void opEvolve(int t){ long long r=findVir(t); MOD(&vLvl[r],vLvl[r]+1); }
static void opAttack(int t){ long long r=findVir(t); MOD(&vTag[r],vTag[r]+vLvl[r]); }

/* ----------- FIX APPLIED HERE ---------------------------------- */
static void opReinstall(int k,int s){
    long long netR = findComp(k);
    long long oldV = findVir(curVir[k]);
    long long newV = findVir(s);

    detachNet(k,netR); MOD(&cSize[netR],cSize[netR]-1);

    if(netR==k && cSize[k]>0){
        long long nr=netHead[k];
        MOD(&cPar[nr],nr); MOD(&cSize[nr],cSize[k]);
        MOD(&cDomVirus[nr],cDomVirus[k]);
        MOD(&netHead[nr],netHead[k]); MOD(&netTail[nr],netTail[k]);
        for(long long u=netHead[nr];u!=NIL;u=nxtNet[u])
            if(u!=nr) MOD(&cPar[u],nr);
        MOD(&cSize[k],0); MOD(&netHead[k],NIL); MOD(&netTail[k],NIL);
    }

    MOD(&cPar[k],k); MOD(&cSize[k],1); MOD(&cDomVirus[k],newV);
    MOD(&netHead[k],k); MOD(&netTail[k],k);

    /* skip list moves if virus unchanged → avoid duplicate node */
    if(oldV != newV){
        detachVir(k,oldV);
        appendVir(newV,k);
    }

    MOD(&baseDmg[k],0);
    MOD(&offset[k],vTag[newV]);
    MOD(&curVir[k],newV);
}
/* --------------------------------------------------------------- */

static void opFusion(int a,int b){
    long long r1=findVir(a), r2=findVir(b); if(r1==r2) return;
    if(vCnt[r1]<vCnt[r2]){long long t=r1;r1=r2;r2=t;}

    MOD(&vPar[r2],r1);
    MOD(&vLvl[r1],vLvl[r1]+vLvl[r2]);
    MOD(&vCnt[r1],vCnt[r1]+vCnt[r2]); MOD(&vCnt[r2],0);

    for(long long u=virHead[r2];u!=NIL;u=nxtVir[u]){
        long long dmg=baseDmg[u]+(vTag[r2]-offset[u]);
        MOD(&baseDmg[u],dmg); MOD(&offset[u],vTag[r1]); MOD(&curVir[u],r1);
    }
    spliceVirusList(r1,r2);
    MOD(&vTag[r2],vTag[r1]);
}

/* ---------- accurate read-only population counter --------------- */
static int seenStamp=1, seen[MAXN];
static long long popCount(long long vRoot){
    long long cnt=0; ++seenStamp;
    for(long long u=virHead[vRoot];u!=NIL;u=nxtVir[u]){
        if(seen[u]==seenStamp) continue;
        seen[u]=seenStamp;
        if(findVir(curVir[u])==vRoot) ++cnt;
    }
    return cnt;
}

/* ---------- status --------------------------------------------- */
static void opStatus(int k){
    long long vR = findVir(curVir[k]);
    long long dmg= baseDmg[k] + (vTag[vR]-offset[k]);
    long long pop= popCount(vR);
    printf("%lld %lld %lld\n",dmg,vLvl[vR],pop);
}

/* ---------------------------------------------------------------- */
int main(void){
    setbuf(stdout,NULL);
    stk = malloc(sizeof(Modify)*cap);

    int N,Q; if(scanf("%d %d",&N,&Q)!=2) return 0;
    for(int i=1;i<=N;i++){
        cPar[i]=i; cSize[i]=1; cDomVirus[i]=i;
        netHead[i]=netTail[i]=i; nxtNet[i]=prvNet[i]=NIL;

        vPar[i]=i; vLvl[i]=1; vCnt[i]=1; vTag[i]=0;
        virHead[i]=virTail[i]=i; nxtVir[i]=prvVir[i]=NIL;

        baseDmg[i]=0; offset[i]=0; curVir[i]=i;
    }

    int tp;
    for(int qi=0; qi<Q; ++qi){
        if(scanf("%d",&tp)!=1) break;
        if(tp!=6 && tp!=7) hist[opId]=0;

        switch(tp){
            case 1:{int x,y; scanf("%d%d",&x,&y); opConnect(x,y); ++opId;} break;
            case 2:{int t;   scanf("%d",&t);       opEvolve(t);   ++opId;} break;
            case 3:{int t;   scanf("%d",&t);       opAttack(t);   ++opId;} break;
            case 4:{int k,s; scanf("%d%d",&k,&s);  opReinstall(k,s); ++opId;} break;
            case 5:{int a,b; scanf("%d%d",&a,&b);  opFusion(a,b); ++opId;} break;
            case 6:{int k;   scanf("%d",&k);       opStatus(k);              } break;
            case 7:{ revertLast();                                        } break;
            default: assert(0);
        }
    }
    return 0;
}
