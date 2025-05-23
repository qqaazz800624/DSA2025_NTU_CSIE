#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY_LEN  65          
#define ALPHABET_SZ  26ULL
#define INIT_R       2           
#define THRESHOLD    2 

typedef struct {
    char               *s;       
    unsigned long long  h;       
} Item;


typedef struct {
    int  *ids;                   
    int   sz;                    
    int   cap;                   
} Bucket;


static Bucket *buckets  = NULL;
static int     nbuckets = 0;     
static int     active   = 0;     

static int r = INIT_R;           
static int q = 0;                

static Item *items = NULL;

static void ensure_bucket_array(int idx);
static void bucket_append(Bucket *b, int id);
static unsigned long long rk_hash(const char *s);
static inline unsigned int lsb_bits(unsigned long long v, int bits);
static int bucket_index(unsigned long long h);
static void split_bucket(void);

int main(){

    int N;
    if (scanf("%d", &N) != 1) return 0;

    items = malloc((size_t)N * sizeof(Item));
    if (!items) { perror("malloc"); exit(EXIT_FAILURE); }

    
    for (int i = 0; i < N; ++i) {
        char buf[MAX_KEY_LEN];
        if (scanf("%64s", buf) != 1) exit(EXIT_FAILURE);
        items[i].s = strdup(buf);
        if (!items[i].s) { perror("strdup"); exit(EXIT_FAILURE); }
        items[i].h = rk_hash(buf);
    }

    ensure_bucket_array(4);
    active = 1 << r;

    for (int i = 0; i < N; ++i) {
        unsigned long long h = items[i].h;
        int idx = bucket_index(h);

        ensure_bucket_array(idx);
        Bucket *b = &buckets[idx];

        bucket_append(b, i);

        if (b->sz > THRESHOLD) {
            ++q;
            split_bucket();
            ++active;
            if (q == (1 << r)) {   
                ++r;
                q = 0;
            }
        }
    }

    
    for (int idx = 0; idx < active; ++idx) {
        Bucket *b = &buckets[idx];
        if (!b->sz) {
            puts("-1");
        } else {
            for (int j = 0; j < b->sz; ++j) {
                if (j) putchar(' ');
                fputs(items[b->ids[j]].s, stdout);
            }
            putchar('\n');
        }
    }
    return 0;
}

static void ensure_bucket_array(int idx)
{
    if (idx < nbuckets) return;
    int new_cap = nbuckets ? nbuckets : 4;
    while (new_cap <= idx) new_cap <<= 1;
    buckets = realloc(buckets, new_cap * sizeof(Bucket));
    if (!buckets) { perror("realloc"); exit(EXIT_FAILURE); }
    for (int i = nbuckets; i < new_cap; ++i)
        buckets[i] = (Bucket){ .ids=NULL, .sz=0, .cap=0 };
    nbuckets = new_cap;
}

static void bucket_append(Bucket *b, int id)
{
    if (b->sz == b->cap) {
        int new_cap = b->cap ? b->cap << 1 : 4;
        b->ids = realloc(b->ids, new_cap * sizeof(int));
        if (!b->ids) { perror("realloc"); exit(EXIT_FAILURE); }
        b->cap = new_cap;
    }
    b->ids[b->sz++] = id;
}

static unsigned long long rk_hash(const char *s)
{
    unsigned long long h = 0;
    for (const char *p = s; *p; ++p)
        h = h * ALPHABET_SZ + (unsigned long long)(*p - 'a');
    return h;                                   
}

static inline unsigned int lsb_bits(unsigned long long v, int bits)
{
    return (unsigned int)(v & ((1ULL << bits) - 1ULL));
}

static int bucket_index(unsigned long long h)    
{
    unsigned int idx = lsb_bits(h, r);           
    if (idx < (unsigned)q)
        idx = lsb_bits(h, r + 1);                
    return (int)idx;
}


static void split_bucket(void)
{
    int split_idx      = q - 1;                  
    int new_bucket_idx = (1 << r) + split_idx;   

    ensure_bucket_array(new_bucket_idx);

    Bucket *old_b = &buckets[split_idx];
    Bucket *new_b = &buckets[new_bucket_idx];

    new_b->sz = 0;   

    int write = 0;   
    for (int i = 0; i < old_b->sz; ++i) {
        int id  = old_b->ids[i];
        unsigned int idx2 = lsb_bits(items[id].h, r + 1); 
        if (idx2 == (unsigned)new_bucket_idx) {
            bucket_append(new_b, id);            
        } else {                                 
            old_b->ids[write++] = id;
        }
    }
    old_b->sz = write;                           
}