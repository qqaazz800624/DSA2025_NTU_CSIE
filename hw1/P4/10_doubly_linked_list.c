#include <stdio.h>
#include <stdlib.h>

typedef long long ll;

typedef struct Node {
    ll value;
    ll diff;
    ll count;
    ll rank;
    struct Node *prev, *next;
} Node;

Node *head = NULL, *tail = NULL;
ll total_count = 0;      
int pending = 0;         
ll M_global = 0;         

void merge_nodes();
void flush_pending();
ll process_removals(ll vi);
void insert_node(int Ni, ll vi);
void process_type_1(int Ni, ll vi);
void process_type_2(ll pi);
void process_type_3();

int main(void) {
    int T;
    scanf("%d %lld", &T, &M_global);
    
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
    flush_pending();
    
    Node *cur = head;
    while (cur) {
        Node *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    return 0;
}

void merge_nodes() {
    Node *cur = head;
    while (cur && cur->next) {
        Node *nxt = cur->next;
        ll last_val = cur->value + (cur->count - 1) * cur->diff;
        if (cur->diff == nxt->diff && ((cur->diff == 0 && cur->value == nxt->value) ||
            (cur->diff != 0 && last_val + cur->diff == nxt->value))) {
            cur->count += nxt->count;
            cur->next = nxt->next;
            if (nxt->next)
                nxt->next->prev = cur;
            else
                tail = cur;
            free(nxt);
        } else {
            cur = cur->next;
        }
    }
}

void flush_pending() {
    if (pending == 0) {
        return;
    }
    ll current_rank = 1;
    Node *cur = head;
    while (cur) {
        cur->rank = current_rank;
        cur->value += pending * (M_global - current_rank + 1);
        if (cur->diff == 0)
            cur->diff = -pending;
        else
            cur->diff -= pending;
        current_rank += cur->count;
        cur = cur->next;
    }
    pending = 0;
    merge_nodes();
}

ll process_removals(ll vi) {
    ll removed = 0;
    Node *cur = tail;
    while (cur) {
        ll smallest = cur->value + (cur->count - 1) * cur->diff;
        if (smallest >= vi) {
            break;
        } else if (cur->value < vi) {
            removed += cur->count;
            total_count -= cur->count;
            Node *prev = cur->prev;
            if (prev)
                prev->next = cur->next;
            else
                head = cur->next;
            if (cur->next)
                cur->next->prev = prev;
            else
                tail = prev;
            Node *tmp = cur;
            cur = prev;
            free(tmp);
        } else {
            int keep = 0;
            if (cur->diff == 0) {
                keep = cur->count;
            } else {
                ll d = -cur->diff;
                ll j = (cur->value - vi) / d; 
                if (j >= cur->count) j = cur->count - 1;
                keep = j + 1;
            }
            removed += (cur->count - keep);
            total_count -= (cur->count - keep);
            cur->count = keep;
            break; 
        }
    }
    return removed;
}

void insert_node(int Ni, ll vi) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->value = vi;
    newNode->diff = 0;  
    newNode->count = Ni;
    newNode->rank = 0;  
    newNode->prev = newNode->next = NULL;
    if (!head) {
        head = tail = newNode;
        total_count += Ni;
        return;
    }
    Node *cur = head;
    Node *insertBefore = NULL;
    while (cur) {
        if (cur->value < vi) {
            insertBefore = cur;
            break;
        }
        cur = cur->next;
    }
    if (insertBefore == NULL) {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    } else {
        newNode->next = insertBefore;
        newNode->prev = insertBefore->prev;
        if (insertBefore->prev)
            insertBefore->prev->next = newNode;
        else
            head = newNode;
        insertBefore->prev = newNode;
    }
    total_count += Ni;
    merge_nodes();
}

void process_type_1(int Ni, ll vi) {
    if (pending > 0) {
        flush_pending();
    }
    ll removed = process_removals(vi);
    printf("%lld\n", removed);
    insert_node(Ni, vi);
}

void process_type_2(ll pi) {
    if (pending > 0){
        flush_pending();
    }
    ll ans = 0;
    Node *cur = head;
    while (cur) {
        if (cur->diff == 0) {
            if (cur->value == pi)
                ans += cur->count;
        } else {
            ll last = cur->value + (cur->count - 1) * cur->diff;
            if (pi <= cur->value && pi >= last) {
                ll d = -cur->diff; 
                if ((cur->value - pi) % d == 0) {
                    ll idx = (cur->value - pi) / d;
                    if (idx < cur->count)
                        ans += 1;
                }
            }
        }
        cur = cur->next;
    }
    printf("%lld\n", ans);
}

void process_type_3() {
    pending++;
}
