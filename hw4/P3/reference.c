/*
 *  Dictionary-Set Administration
 *  — full AC solution for all 4 subtasks
 *
 *  Implements all six required operations on a Trie.
 *  The “Compress” operation is solved optimally with a
 *  bottom-up pass that uses max-heaps to keep the current
 *  multiset of chosen prefix-depths inside each subtree.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ALPHA       26
#define MAX_NODES   310000          /* total input ≤ 3 × 10^5 letters   */
#define MAX_Q       10000

/* ------------------------------------------------------------- */
/*                           TRIE NODE                           */
/* ------------------------------------------------------------- */
typedef struct {
    int  ch[ALPHA];     /* children indices (–1 if absent)        */
    int  cnt;           /* #words in the subtree below (incl. me) */
    bool term;          /* a word ends exactly at this node       */
} Node;

static Node trie[MAX_NODES];
static int  nodeCnt = 1;            /* 0 = root                     */

static int newNode(void)
{
    for (int i = 0; i < ALPHA; ++i) trie[nodeCnt].ch[i] = -1;
    trie[nodeCnt].cnt  = 0;
    trie[nodeCnt].term = false;
    return nodeCnt++;
}

/* ------------------------------------------------------------- */
/*                      BASIC TRIE OPERATIONS                    */
/* ------------------------------------------------------------- */
static void insert(const char *s)
{
    int v = 0;
    ++trie[v].cnt;                          /* root counts too        */
    for (const char *p = s; *p; ++p) {
        int c = *p - 'a';
        if (trie[v].ch[c] == -1) trie[v].ch[c] = newNode();
        v = trie[v].ch[c];
        ++trie[v].cnt;
    }
    trie[v].term = true;
}

static bool findWord(const char *s)
{
    int v = 0;
    for (const char *p = s; *p; ++p) {
        int c = *p - 'a';
        if (trie[v].ch[c] == -1) return false;
        v = trie[v].ch[c];
    }
    return trie[v].term;
}

static int prefixCount(const char *p)
{
    int v = 0;
    for (const char *q = p; *q; ++q) {
        int c = *q - 'a';
        if (trie[v].ch[c] == -1) return 0;
        v = trie[v].ch[c];
    }
    return trie[v].cnt;
}

/* ------------------------------------------------------------- */
/*                LEXICOGRAPHIC-FIRST DFS FOR LCP                */
/* ------------------------------------------------------------- */
static bool dfsFirst(int v, char *buf, int depth)
{
    if (trie[v].term) { buf[depth] = '\0'; return true; }
    for (int c = 0; c < ALPHA; ++c) {
        int nxt = trie[v].ch[c];
        if (nxt != -1) {
            buf[depth] = 'a' + c;
            if (dfsFirst(nxt, buf, depth + 1)) return true;
        }
    }
    return false;
}

static void queryLCP(const char *q, char *out)
{
    int v = 0, depth = 0;
    for (const char *p = q; *p; ++p) {
        int c = *p - 'a';
        if (trie[v].ch[c] == -1) break;
        out[depth++] = *p;
        v = trie[v].ch[c];
    }
    dfsFirst(v, out, depth);
}

/* ------------------------------------------------------------- */
/*                        SCORE OPERATION                        */
/* ------------------------------------------------------------- */
static long long queryScore(const char *q)
{
    int v = 0, depth = 0;
    long long total = 0;
    int len = (int)strlen(q);

    while (depth < len) {
        int c = q[depth] - 'a';
        int child = trie[v].ch[c];
        if (child == -1) {                      /* diverges completely */
            total += (long long)trie[v].cnt * depth;
            return total;
        }
        int diverge = trie[v].cnt - trie[child].cnt;
        total += (long long)diverge * depth;    /* end at current v   */
        v = child;
        ++depth;
    }
    total += (long long)trie[v].cnt * depth;    /* prefixes == q      */
    return total;
}

/* ------------------------------------------------------------- */
/*     OPTIMAL “COMPRESS” WITH BOTTOM-UP HEAP CONSOLIDATION      */
/* ------------------------------------------------------------- */
typedef struct {
    int   *key;         /* max-heap of depths                      */
    int    sz, cap;
    long long sum;      /* Σ depths currently in the heap         */
} Heap;

static Heap *newHeap(void)
{
    Heap *h = (Heap*)malloc(sizeof(Heap));
    h->cap = 4; h->sz = 0; h->sum = 0;
    h->key = (int*)malloc(sizeof(int) * h->cap);
    return h;
}

static void heapPush(Heap *h, int v)
{
    if (h->sz == h->cap) {
        h->cap <<= 1;
        h->key = (int*)realloc(h->key, sizeof(int) * h->cap);
    }
    int i = h->sz++;
    h->sum += v;
    while (i) {
        int p = (i - 1) >> 1;
        if (h->key[p] >= v) break;
        h->key[i] = h->key[p];
        i = p;
    }
    h->key[i] = v;
}

static int heapPop(Heap *h)          /* assumes h->sz > 0 */
{
    int ret = h->key[0];
    h->sum -= ret;
    int last = h->key[--h->sz];
    int i = 0;
    while ((i << 1) + 1 < h->sz) {
        int l = (i << 1) + 1, r = l + 1, big = l;
        if (r < h->sz && h->key[r] > h->key[l]) big = r;
        if (last >= h->key[big]) break;
        h->key[i] = h->key[big];
        i = big;
    }
    if (h->sz) h->key[i] = last;
    return ret;
}

static void heapMerge(Heap *dst, Heap *src)
{
    for (int i = 0; i < src->sz; ++i) heapPush(dst, src->key[i]);
    free(src->key); free(src);
}

/* DFS returns a heap containing the optimal compressed
   prefix-lengths for the subtree rooted at v.                        */
static Heap *dfsCompress(int v, int depth)
{
    Heap *h = newHeap();
    if (trie[v].term) heapPush(h, depth);           /* word ends here */

    for (int c = 0; c < ALPHA; ++c)
        if (trie[v].ch[c] != -1) {
            Heap *sub = dfsCompress(trie[v].ch[c], depth + 1);
            /* union-by-size: keep larger heap as destination        */
            if (h->sz < sub->sz) { Heap *tmp = h; h = sub; sub = tmp; }
            heapMerge(h, sub);
        }

    /* internal non-terminal node (depth>0) can “steal” one prefix   */
    if (depth > 0 && !trie[v].term) {
        heapPop(h);             /* remove deepest prefix in subtree  */
        heapPush(h, depth);     /* replace with shorter current one  */
    }
    return h;
}

static long long runCompress(void)
{
    Heap *root = dfsCompress(0, 0);
    long long ans = root->sum;
    free(root->key); free(root);
    return ans;
}

/* ------------------------------------------------------------- */
/*                             MAIN                              */
/* ------------------------------------------------------------- */
int main(void)
{
    /* initialise the root */
    for (int i = 0; i < ALPHA; ++i) trie[0].ch[i] = -1;
    trie[0].cnt = 0; trie[0].term = false;

    int Q;
    if (scanf("%d", &Q) != 1) return 0;

    char buffer[10005], answer[10005];

    while (Q--) {
        int op;
        scanf("%d", &op);

        if (op == 1) {                    /* Insert */
            scanf("%s", buffer);
            insert(buffer);

        } else if (op == 2) {             /* Check  */
            scanf("%s", buffer);
            puts(findWord(buffer) ? "YES" : "NO");

        } else if (op == 3) {             /* Prefix_Search */
            scanf("%s", buffer);
            printf("%d\n", prefixCount(buffer));

        } else if (op == 4) {             /* LCP */
            scanf("%s", buffer);
            queryLCP(buffer, answer);
            puts(answer);

        } else if (op == 5) {             /* Score */
            scanf("%s", buffer);
            printf("%lld\n", queryScore(buffer));

        } else if (op == 6) {             /* Compress (always last) */
            printf("%lld\n", runCompress());
        }
    }
    return 0;
}
