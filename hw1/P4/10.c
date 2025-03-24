#include <stdio.h>

#define MAX_CAP 1000000
typedef long long ll;

typedef struct {
    ll value;
    ll count;
    ll rank;
    ll initial_type3;
} Node;

Node nodes[MAX_CAP];
int size = 0;
ll pending = 0;
ll M;
ll acc[MAX_CAP];

void update_acc(int start) {
    if (size == 0) return;
    if (start == 0) {
        acc[0] = nodes[0].count;
        nodes[0].rank = 1;
        start = 1;
    }
    for (int i = start; i < size; i++) {
        acc[i] = acc[i - 1] + nodes[i].count;
        nodes[i].rank = acc[i - 1] + 1;
    }
}

ll get_current_value(int idx) {
    return nodes[idx].value + (pending - nodes[idx].initial_type3) * (M - nodes[idx].rank + 1);
}

ll get_current_diff(int idx) {
    return pending - nodes[idx].initial_type3;
}

int binary_search(ll v) {
    int low = 0, high = size - 1, result = size; 
    while (low <= high) {
        int mid = (low + high) / 2;
        ll cur_val = get_current_value(mid);
        ll diff = get_current_diff(mid);
        ll last_val = cur_val - (nodes[mid].count - 1) * diff;

        if (last_val < v) {
            result = mid;
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return result;
}

void op1(int N, ll v) {
    int idx = binary_search(v);
    ll removed = 0;

     // Check partially overlapped node at idx first (partial removal)
     if (idx < size) {
        ll cur_val = get_current_value(idx);
        ll diff = get_current_diff(idx);
        ll last_val = cur_val - (nodes[idx].count - 1) * diff;

        if (cur_val >= v && last_val < v && diff != 0) {
            ll keep = (cur_val - v) / diff + 1;
            if (keep < nodes[idx].count) {
                removed += nodes[idx].count - keep;
                nodes[idx].count = keep;
            }
            update_acc(idx);
            idx++; // we've handled idx partially, now remove from idx+1 onwards completely
        }
    }

    // Completely remove nodes after the idx (already handled partial at idx)
    if (idx < size) {
        removed += acc[size - 1] - (idx > 0 ? acc[idx - 1] : 0);
        size = idx;
    }

    printf("%lld\n", removed);

    // Merge new node
    if (size > 0) {
        ll top_val = get_current_value(size - 1);
        ll top_diff = get_current_diff(size - 1);
        if (top_val == v && top_diff == 0) {
            nodes[size - 1].count += N;
            nodes[size - 1].initial_type3 = pending;
            update_acc(size - 1);
            return;
        }
    }

    // insert new node
    nodes[size].value = v;
    nodes[size].count = N;
    nodes[size].initial_type3 = pending;
    size++;

    update_acc(size - 1);
}

void op2(ll p) {
    ll ans = 0;
    ll low = 0, high = size - 1;
    ll mid = -1;
    while (low <= high) {
        mid = (low + high) / 2;
        ll cur_val = get_current_value(mid);
        if (cur_val == p) {
            break;
        }
        if (cur_val < p) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    for (ll i = mid - 1; i <= mid + 1; i++) {
        if (i < 0 || i >= size) continue;
        ll cur_val = get_current_value(i);
        ll diff = get_current_diff(i);
        ll last_val = cur_val - (nodes[i].count - 1) * diff;

        if (cur_val >= p && last_val <= p) {
            if (diff == 0 && cur_val == p) {
                ans += nodes[i].count;
            } else if (diff != 0 && (cur_val - p) % diff == 0) {
                ans += 1;
            }
        }
    }

    printf("%lld\n", ans);
}



void op3() {
    pending++;
}

int main() {
    int T;
    scanf("%d %lld", &T, &M);

    for (int i = 0; i < T; i++) {
        int op;
        scanf("%d", &op);
        if (op == 1) {
            int N;
            ll v;
            scanf("%d %lld", &N, &v);
            op1(N, v);
        } else if (op == 2) {
            ll p;
            scanf("%lld", &p);
            op2(p);
        } else if (op == 3) {
            op3();
        }
    }
    return 0;
}
