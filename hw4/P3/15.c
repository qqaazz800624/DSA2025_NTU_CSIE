#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ALPHA       26
#define MAX_NODES   310000          
#define MAX_Q       10000

typedef struct {
    int  ch[ALPHA];     
    int  cnt;           
    bool term;          
} Node;

static Node trie[MAX_NODES];
static int  nodeCnt = 1;

typedef struct {
    int   *key;         
    int    sz, cap;
    long long sum;      
} Heap;

static int newNode(void);
static void insert(const char *s);
static bool findWord(const char *s);
static int prefixCount(const char *p);
static bool dfsFirst(int v, char *buf, int depth);
static void queryLCP(const char *q, char *out);
static long long queryScore(const char *q);
static Heap *newHeap(void);
static void heapPush(Heap *h, int v);
static int heapPop(Heap *h);
static void heapMerge(Heap *dst, Heap *src);
static Heap *dfsCompress(int v, int depth);
static long long runCompress(void);

int main()
{
    for (int i = 0; i < ALPHA; ++i) trie[0].ch[i] = -1;
    trie[0].cnt = 0; trie[0].term = false;

    int Q;
    if (scanf("%d", &Q) != 1) return 0;

    char buffer[10005], answer[10005];

    while (Q--) {
        int op;
        scanf("%d", &op);

        if (op == 1) {                    
            scanf("%s", buffer);
            insert(buffer);

        } else if (op == 2) {             
            scanf("%s", buffer);
            puts(findWord(buffer) ? "YES" : "NO");

        } else if (op == 3) {             
            scanf("%s", buffer);
            printf("%d\n", prefixCount(buffer));

        } else if (op == 4) {             
            scanf("%s", buffer);
            queryLCP(buffer, answer);
            puts(answer);

        } else if (op == 5) {             
            scanf("%s", buffer);
            printf("%lld\n", queryScore(buffer));

        } else if (op == 6) {             
            printf("%lld\n", runCompress());
        }
    }
    return 0;
}

static int newNode(void)
{
    for (int i = 0; i < ALPHA; ++i) trie[nodeCnt].ch[i] = -1;
    trie[nodeCnt].cnt  = 0;
    trie[nodeCnt].term = false;
    return nodeCnt++;
}

static void insert(const char *s)
{
    int v = 0;
    ++trie[v].cnt;                          
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

static long long queryScore(const char *q)
{
    int v = 0, depth = 0;
    long long total = 0;
    int len = (int)strlen(q);

    while (depth < len) {
        int c = q[depth] - 'a';
        int child = trie[v].ch[c];
        if (child == -1) {                      
            total += (long long)trie[v].cnt * depth;
            return total;
        }
        int diverge = trie[v].cnt - trie[child].cnt;
        total += (long long)diverge * depth;    
        v = child;
        ++depth;
    }
    total += (long long)trie[v].cnt * depth;    
    return total;
}

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

static int heapPop(Heap *h)          
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


static Heap *dfsCompress(int v, int depth)
{
    Heap *h = newHeap();
    if (trie[v].term) heapPush(h, depth);           

    for (int c = 0; c < ALPHA; ++c)
        if (trie[v].ch[c] != -1) {
            Heap *sub = dfsCompress(trie[v].ch[c], depth + 1);
            if (h->sz < sub->sz) { Heap *tmp = h; h = sub; sub = tmp; }
            heapMerge(h, sub);
        }

    if (depth > 0 && !trie[v].term) {
        heapPop(h);             
        heapPush(h, depth);     
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