#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXN 300005
#define MAX_LOG 20

typedef unsigned long long ull;

int n, m, q;

// Tree adjacency
int head[MAXN], to[2 * MAXN], nxt[2 * MAXN], w[2 * MAXN], eidx;
int up[MAXN][MAX_LOG], depth[MAXN];
long long dist[MAXN];

int c[MAXN]; // capacities

// Data structure for each parked bike in a slot
struct pos {
    int s;       // student id
    ull num, den;// fractional position
};

struct pos slots[MAXN][30];
int slot_size[MAXN]; // how many bikes are in slot x

int student_slot[MAXN]; // which slot does a student's bike currently occupy (-1 if none)

// each student s has a parameter ls[s]
ull ls[MAXN];

// For Clear/Rearrange notifications
struct notif {
    ull t;
    int s; // student
};
struct notif all_notifs[600000]; // Buffer for notifications
int notif_count = 0;

// We'll keep a pointer for how many notifications have been permanently fetched
int fetched_ptr = 0;

ull gcd(ull a, ull b) {
    return b ? gcd(b, a % b) : a;
}

void add_edge(int u, int v, int ww) {
    to[eidx] = v; w[eidx] = ww; nxt[eidx] = head[u]; head[u] = eidx++;
    to[eidx] = u; w[eidx] = ww; nxt[eidx] = head[v]; head[v] = eidx++;
}

void dfs(int v, int p) {
    up[v][0] = p;
    for (int i = 1; i < MAX_LOG; i++) {
        up[v][i] = up[ up[v][i-1] ][i-1];
    }
    for (int e = head[v]; e != -1; e = nxt[e]) {
        int u = to[e];
        if (u == p) continue;
        depth[u] = depth[v] + 1;
        dist[u] = dist[v] + w[e];
        dfs(u, v);
    }
}

int lca(int a, int b) {
    if (depth[a] < depth[b]) {
        int tmp = a; a = b; b = tmp;
    }
    for (int i = MAX_LOG - 1; i >= 0; i--) {
        if (depth[ up[a][i] ] >= depth[b]) {
            a = up[a][i];
        }
    }
    if (a == b) return a;
    for (int i = MAX_LOG - 1; i >= 0; i--) {
        if (up[a][i] != up[b][i]) {
            a = up[a][i];
            b = up[b][i];
        }
    }
    return up[a][0];
}

long long get_dist(int a, int b) {
    return dist[a] + dist[b] - 2LL * dist[lca(a, b)];
}

// Insert a new notification (time t, student s) into all_notifs in ascending order of t
// O(notif_count) insertion, which is typically fine for q <= 1e5.
void insert_notif(ull t, int s) {
    int pos = notif_count;
    // Move from the end backward while the preceding has a larger time
    while (pos > 0 && all_notifs[pos - 1].t > t) {
        all_notifs[pos] = all_notifs[pos - 1];
        pos--;
    }
    all_notifs[pos].t = t;
    all_notifs[pos].s = s;
    notif_count++;
}

// Attempt to park the bike for student s in slot x at position p
// We apply the four rules from the problem statement.
void insert_bike(int s, int x, ull pnum, ull pden, int output) {
    int sz = slot_size[x];
    slots[x][sz].s   = s;
    slots[x][sz].num = pnum;
    slots[x][sz].den = pden;
    slot_size[x]++;
    student_slot[s] = x;

    // simplify the fraction
    ull g = gcd(pnum, pden);
    pnum /= g; 
    pden /= g;

    if (output) {
        if (pden == 1ULL) {
            printf("%d parked at (%d, %llu).\n", s, x, pnum);
        } else {
            printf("%d parked at (%d, %llu/%llu).\n", s, x, pnum, pden);
        }
    }
}

// Compare two parked bikes by numeric value (num/den)
int cmp_pos(const void *aa, const void *bb) {
    const struct pos *a = (const struct pos *)aa;
    const struct pos *b = (const struct pos *)bb;
    __uint128_t lhs = (__uint128_t)a->num * b->den;
    __uint128_t rhs = (__uint128_t)b->num * a->den;
    if (lhs < rhs) return -1;
    if (lhs > rhs) return 1;
    return 0;
}

// Park operation, applying rules (1) -> (4)
void park_bike(int s, int x, int p, int output) {
    // Mark integer positions that are occupied
    int used[20]; 
    memset(used, 0, sizeof(used));
    for (int i = 0; i < slot_size[x]; i++) {
        if (slots[x][i].den == 1) {
            int val = (int)slots[x][i].num;
            if (val >= 1 && val <= c[x]) used[val] = 1;
        }
    }
    // Rule (1): if p is free
    if (!used[p]) {
        insert_bike(s, x, p, 1ULL, output);
        return;
    }
    // Rule (2): find another free integer with minimal distance from p
    int best = -1, bestDiff = 9999;
    for (int i = 1; i <= c[x]; i++) {
        if (!used[i]) {
            int diff = (i > p) ? (i - p) : (p - i);
            if (diff < bestDiff || (diff == bestDiff && i < best)) {
                best = i; 
                bestDiff = diff;
            }
        }
    }
    if (best != -1) {
        insert_bike(s, x, best, 1ULL, output);
        return;
    }
    // Otherwise, fractional insertion
    qsort(slots[x], slot_size[x], sizeof(struct pos), cmp_pos);

    // occupant exactly p/1 ?
    int index_p = -1;
    for (int i = 0; i < slot_size[x]; i++) {
        if (slots[x][i].den == 1 && slots[x][i].num == (ull)p) {
            index_p = i;
            break;
        }
    }
    if (index_p != -1) {
        // if not smallest => rule(3), else => rule(4)
        if (index_p > 0) {
            ull a = slots[x][index_p - 1].num, b = slots[x][index_p - 1].den;
            ull new_num = (ull)p * b + a;
            ull new_den = 2ULL * b;
            insert_bike(s, x, new_num, new_den, output);
        } else {
            ull a = slots[x][index_p + 1].num, b = slots[x][index_p + 1].den;
            ull new_num = (ull)p * b + a;
            ull new_den = 2ULL * b;
            insert_bike(s, x, new_num, new_den, output);
        }
        return;
    }
    // occupant p not found => find insertion index
    ull p_num = (ull)p, p_den = 1ULL;
    int idx = -1;
    for (int i = 0; i < slot_size[x]; i++) {
        __uint128_t lhs = (__uint128_t)p_num * slots[x][i].den;
        __uint128_t rhs = (__uint128_t)slots[x][i].num * p_den;
        if (lhs < rhs) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        // p/1 bigger than all occupant
        int last = slot_size[x] - 1;
        ull a = slots[x][last].num, b = slots[x][last].den;
        ull new_num = p_num * b + a;
        ull new_den = 2ULL * b;
        insert_bike(s, x, new_num, new_den, output);
    } else if (idx == 0) {
        // p/1 smaller than occupant[0]
        ull a = slots[x][0].num, b = slots[x][0].den;
        ull new_num = p_num * b + a;
        ull new_den = 2ULL * b;
        insert_bike(s, x, new_num, new_den, output);
    } else {
        // rule(3) effectively => occupant at idx-1
        ull a = slots[x][idx - 1].num, b = slots[x][idx - 1].den;
        ull new_num = p_num * b + a;
        ull new_den = 2ULL * b;
        insert_bike(s, x, new_num, new_den, output);
    }
}

// We'll define do_fetch(t) to fetch all notifications up to time t
// We'll count how many new ones we fetch, ignoring the ones we've already accounted for with 'fetched_ptr'.
void do_fetch(ull t) {
    int cnt = 0;
    // move fetched_ptr forward while all_notifs[fetched_ptr].t <= t
    while (fetched_ptr < notif_count && all_notifs[fetched_ptr].t <= t) {
        fetched_ptr++;
        cnt++;
    }
    printf("At %llu, %d bikes was fetched.\n", t, cnt);
}

int main() {
    memset(head, -1, sizeof(head));
    scanf("%d%d%d", &n, &m, &q);

    for (int i = 0; i < n; i++)
        scanf("%d", &c[i]);

    for (int i = 0; i < m; i++) {
        scanf("%llu", &ls[i]);
        student_slot[i] = -1; // not parked
    }

    for (int i = 0, u, v, ww; i < n - 1; i++) {
        scanf("%d%d%d", &u, &v, &ww);
        add_edge(u, v, ww);
    }
    dfs(0, 0);

    for (int i = 0; i < q; i++) {
        int op;
        scanf("%d", &op);
        if (op == 0) {
            // Park
            int s, x, p;
            scanf("%d%d%d", &s, &x, &p);
            park_bike(s, x, p, 1);
        }
        else if (op == 1) {
            // Move
            int s, x, p;
            scanf("%d%d%d", &s, &x, &p);
            int src = student_slot[s];
            if (src == x) {
                // same slot => no travel time
                printf("%d moved to %d in 0 seconds.\n", s, x);
                continue;
            }
            // remove from src if currently in a slot
            if (src != -1) {
                int posIndex = -1;
                for (int j = 0; j < slot_size[src]; j++) {
                    if (slots[src][j].s == s) {
                        posIndex = j;
                        break;
                    }
                }
                if (posIndex != -1) {
                    // shift
                    for (int j = posIndex; j < slot_size[src] - 1; j++)
                        slots[src][j] = slots[src][j + 1];
                    slot_size[src]--;
                }
            }
            // park in new slot
            park_bike(s, x, p, 0);
            long long timeCost = 0;
            if (src != -1 && src != x) {
                timeCost = get_dist(src, x);
            }
            printf("%d moved to %d in %lld seconds.\n", s, x, timeCost);
        }
        else if (op == 2) {
            // Clear
            int x; 
            ull Tclear;
            scanf("%d%llu", &x, &Tclear);
            // remove all bikes from slot x
            for (int j = 0; j < slot_size[x]; j++) {
                int st = slots[x][j].s;
                if (student_slot[st] == x) {
                    insert_notif(Tclear + ls[st], st); // schedule notification
                    student_slot[st] = -1; // no longer in a slot
                }
            }
            slot_size[x] = 0;
        }
        else if (op == 3) {
            // Rearrange
            int x; 
            ull Trr;
            scanf("%d%llu", &x, &Trr);
            int cnt = 0;
            struct pos newSlots[30];
            int newCount = 0;
            for (int j = 0; j < slot_size[x]; j++) {
                int st = slots[x][j].s;
                if (slots[x][j].den > 1ULL && student_slot[st] == x) {
                    insert_notif(Trr + ls[st], st);
                    student_slot[st] = -1;
                    cnt++;
                } else {
                    newSlots[newCount++] = slots[x][j];
                }
            }
            for (int j = 0; j < newCount; j++)
                slots[x][j] = newSlots[j];
            slot_size[x] = newCount;
            printf("Rearranged %d bikes in %d.\n", cnt, x);
        }
        else if (op == 4) {
            // Fetch
            ull T; 
            scanf("%llu", &T);
            // fetch up to time T
            do_fetch(T);
        }
        else if (op == 5) {
            // Rebuild
            int x, y, d;
            scanf("%d%d%d", &x, &y, &d);
            for (int e = head[x]; e != -1; e = nxt[e]) {
                if (to[e] == y) w[e] = d;
            }
            for (int e = head[y]; e != -1; e = nxt[e]) {
                if (to[e] == x) w[e] = d;
            }
            dfs(0, 0);
        }
    }
    return 0;
}
