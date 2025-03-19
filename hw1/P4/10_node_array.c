#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef long long ll;

typedef struct Node {
    ll value;  
    ll diff;   
    ll count;  
    ll rank;  
} Node;

typedef struct {
    Node **data;
    int size;
    int capacity;
} NodeArray;

NodeArray segArr;
ll total_count = 0;
int pending = 0;
ll M_global = 0;

void initArray(NodeArray *arr);
void merge_segments(NodeArray *arr);
void flush_pending(NodeArray *arr);
int find_boundary(NodeArray *arr, ll v);
void pop_from_index(NodeArray *arr, int idx, ll *removed);
ll process_removals(NodeArray *arr, ll v);
int lower_bound(NodeArray *arr, ll v);
void resizeArray(NodeArray *arr, int new_capacity);
void insert_node(NodeArray *arr, int Ni, ll v);
void process_type_1(int Ni, ll v);
int binary_search_candidate(NodeArray *arr, ll p);
void process_type_2(ll p);
void process_type_3();
void freeArray(NodeArray *arr);

int main(void) {
    int T;
    scanf("%d %lld", &T, &M_global);
    initArray(&segArr);
    srand((unsigned)time(NULL));
    
    for (int i = 0; i < T; i++) {
        int op;
        scanf("%d", &op);
        if (op == 1) {
            int Ni;
            ll vi;
            scanf("%d %lld", &Ni, &vi);
            process_type_1(Ni, vi);
        } else if (op == 2) {
            ll pi;
            scanf("%lld", &pi);
            process_type_2(pi);
        } else if (op == 3) {
            process_type_3();
        }
    }
    flush_pending(&segArr);
    freeArray(&segArr);
    return 0;
}

void initArray(NodeArray *arr) {
    arr->capacity = 1024;
    arr->size = 0;
    arr->data = (Node**)malloc(arr->capacity * sizeof(Node*));
}

void merge_segments(NodeArray *arr) {
    if (arr->size == 0) return;
    int j = 0;
    for (int i = 0; i < arr->size; i++) {
        if (j == 0) {
            arr->data[j++] = arr->data[i];
        } else {
            Node *prev = arr->data[j - 1];
            Node *curr = arr->data[i];
            if (prev->diff == curr->diff) {
                if (prev->diff == 0) {
                    if (prev->value == curr->value) {
                        prev->count += curr->count;
                        free(curr);
                    } else {
                        arr->data[j++] = curr;
                    }
                } else {
                    ll last_val = prev->value + (prev->count - 1) * prev->diff;
                    if (last_val + prev->diff == curr->value) {
                        prev->count += curr->count;
                        free(curr);
                    } else {
                        arr->data[j++] = curr;
                    }
                }
            } else {
                arr->data[j++] = curr;
            }
        }
    }
    arr->size = j;
}

void flush_pending(NodeArray *arr){
    if (pending == 0) return;
    ll current_rank = 1;
    for (int i = 0; i < arr->size; i++) {
        Node *node = arr->data[i];
        node->rank = current_rank;
        node->value += pending * (M_global - current_rank + 1);
        if (node->diff == 0)
            node->diff = -pending;
        else
            node->diff -= pending;
        current_rank += node->count;
    }
    pending = 0;
    merge_segments(arr);
}

int find_boundary(NodeArray *arr, ll v) {
    int lo = 0, hi = arr->size;
    while (lo < hi) {
        int mid = (lo + hi) / 2;
        if (arr->data[mid]->value >= v)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

void pop_from_index(NodeArray *arr, int idx, ll *removed) {
    for (int i = idx; i < arr->size; i++) {
        *removed += arr->data[i]->count;
        total_count -= arr->data[i]->count;
        free(arr->data[i]);
    }
    arr->size = idx;
}

ll process_removals(NodeArray *arr, ll v) {
    ll removed = 0;
    int boundary = find_boundary(arr, v);
    if (boundary > 0) {
        Node *node = arr->data[boundary - 1];
        ll last = node->value + (node->count - 1) * node->diff;
        if (node->value >= v && last < v && node->diff != 0) {
            ll d = -node->diff; 
            ll k = (node->value - v) / d + 1;
            if (k > node->count) k = node->count;
            removed += (node->count - k);
            total_count -= (node->count - k);
            node->count = k;
        }
    }
    pop_from_index(arr, boundary, &removed);
    return removed;
}

int lower_bound(NodeArray *arr, ll v) {
    int lo = 0, hi = arr->size;
    while (lo < hi) {
        int mid = (lo + hi) / 2;
        if (arr->data[mid]->value >= v)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

void resizeArray(NodeArray *arr, int new_capacity) {
    arr->data = (Node**)realloc(arr->data, new_capacity * sizeof(Node*));
    arr->capacity = new_capacity;
}

void insert_node(NodeArray *arr, int Ni, ll v){
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->value = v;
    newNode->diff = 0;
    newNode->count = Ni;
    newNode->rank = 0;
    int pos = lower_bound(arr, v);
    if (arr->size == arr->capacity)
        resizeArray(arr, arr->capacity * 2);
    for (int i = arr->size; i > pos; i--) {
        arr->data[i] = arr->data[i-1];
    }
    arr->data[pos] = newNode;
    arr->size++;
    total_count += Ni;
    merge_segments(arr);
}

void process_type_1(int Ni, ll v){
    if (pending > 0)
        flush_pending(&segArr);
    ll rem = process_removals(&segArr, v);
    printf("%lld\n", rem);
    insert_node(&segArr, Ni, v);
}

int binary_search_candidate(NodeArray *arr, ll p){
    int lo = 0, hi = arr->size - 1;
    int candidate = -1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (arr->data[mid]->value >= p) {
            candidate = mid;  
            lo = mid + 1;    
        } else {
            hi = mid - 1;
        }
    }
    return candidate;
}

void process_type_2(ll p) {
    if (pending > 0)
        flush_pending(&segArr);
    ll ans = 0;
    
    int candidate = binary_search_candidate(&segArr, p);
    for (int i = candidate - 1; i <= candidate + 1; i++) {
        if (i < 0 || i >= segArr.size)
            continue;
        Node *node = segArr.data[i];
        if (node->diff == 0) {
            if (node->value == p)
                ans += node->count;
        } else {
            ll last = node->value + (node->count - 1) * node->diff;
            if (p <= node->value && p >= last) {
                ll diff = -node->diff; 
                if ((node->value - p) % diff == 0) {
                    ll pos = (node->value - p) / diff;
                    if (pos < node->count)
                        ans += 1;
                }
            }
        }
    }
    printf("%lld\n", ans);
}

void process_type_3(){
    pending++;
}

void freeArray(NodeArray *arr) {
    for (int i = 0; i < arr->size; i++) {
        free(arr->data[i]);
    }
    free(arr->data);
}