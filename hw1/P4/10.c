#include <stdio.h>
#include <stdlib.h>

#define INIT_SIZE 100000

typedef long long ll;

typedef struct {
    long long *value;
    int size;
    int capacity;
} Diamonds;

void init(Diamonds *d);
void resize(Diamonds *d, int new_capacity);
void process_type_1(Diamonds *d, int Ni, long long vi);
int binary_search(Diamonds *d, long long key);
void process_type_2(Diamonds *d, long long pi);
int cmp_desc(const void *a, const void *b);
void process_type_3(Diamonds *d, long long M);

int main() {
    int T; long long M;
    scanf("%d %lld", &T, &M);
    Diamonds d;
    init(&d);

    for (int i = 0; i < T; i++) {
        int op; 
        scanf("%d", &op);
        if (op == 1) {
            int Ni; long long vi;
            scanf("%d %lld", &Ni, &vi);
            process_type_1(&d, Ni, vi);
        } else if (op == 2) {
            long long pi;
            scanf("%lld", &pi);
            process_type_2(&d, pi);
        } else {
            process_type_3(&d, M);
        }
    }
    free(d.value);
    return 0;
}

void init(Diamonds *d) {
    d->capacity = INIT_SIZE;
    d->size = 0;
    d->value = (long long *)malloc(d->capacity * sizeof(long long));
    if (d->value == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

void resize(Diamonds *d, int new_capacity) {
    d->capacity = new_capacity;
    d->value = realloc(d->value, d->capacity * sizeof(long long));
    if (d->value == NULL) {
        fprintf(stderr, "Memory reallocation failed\n");
        exit(1);
    }
}

void process_type_1(Diamonds *d, int Ni, long long vi) {
    int new_size = 0, removed = 0; 
    for (int i = 0; i < d->size; i++) {
        if (d->value[i] >= vi) {
            d->value[new_size++] = d->value[i];
        } else {
            removed++;
        }
    }
    printf("%d\n", removed);
    d->size = new_size; 

    if (d->size + Ni > d->capacity){
        int new_capacity = d->capacity;
        while (d->size + Ni > new_capacity)
            new_capacity *= 2;
        resize(d, new_capacity);
    }

    for (int i = 0; i < Ni; i++) {
        d->value[d->size++] = vi;
    }
}

int binary_search(Diamonds *d, long long key) {
    int left = 0, right = d->size - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (d->value[mid] == key) 
            return mid;
        else if (d->value[mid] < key)
            right = mid - 1;
        else
            left = mid + 1;
    }
    return -1;
}

void process_type_2(Diamonds *d, long long pi) {
    int idx = binary_search(d, pi);
    if (idx == -1) { 
        printf("0\n"); 
        return; 
    }
    int cnt = 1, l = idx - 1, r = idx + 1;
    while (l >= 0 && d->value[l--] == pi) {
        cnt++;
    }
    while (r < d->size && d->value[r++] == pi) {
        cnt++;
    }
    printf("%d\n", cnt);
}

int cmp_desc(const void *a, const void *b) {
    long long x = *(const long long*)a, y = *(const long long*)b;
    return (x < y) - (x > y);  
}

void process_type_3(Diamonds *d, long long M) {
    qsort(d->value, d->size, sizeof(long long), cmp_desc);
    for (int i = 0; i < d->size; i++)
        d->value[i] += (M - i);
}

