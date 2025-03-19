#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef long long ll;

//////////////////////////////////////////////
// Node 定義：儲存一段連續鑽石資料
//////////////////////////////////////////////
typedef struct Node {
    ll value;  // 此段第一顆鑽石的值
    ll diff;   // 相鄰鑽石的差（若 diff == 0 則整段皆相同）
    ll count;  // 此段鑽石數
    ll rank;   // 此段第一顆鑽石在全局排序中的排名（flush_pending 時更新）
} Node;

//////////////////////////////////////////////
// 動態陣列 (stack) 定義：存放 Node*，按鑽石值由大到小排序
//////////////////////////////////////////////
typedef struct {
    Node **data;
    int size;
    int capacity;
} NodeArray;

NodeArray segArr;
ll total_count = 0;  // 全部鑽石數
int pending = 0;     // 累積的 type 3 更新數量
ll M_global = 0;     // 給定常數 M

//////////////////////////////////////////////
// 動態陣列操作
//////////////////////////////////////////////
void initArray(NodeArray *arr) {
    arr->capacity = 1000000;
    arr->size = 0;
    arr->data = (Node**)malloc(arr->capacity * sizeof(Node*));
}

void freeArray(NodeArray *arr) {
    for (int i = 0; i < arr->size; i++) {
        free(arr->data[i]);
    }
    free(arr->data);
}

void resizeArray(NodeArray *arr, int new_capacity) {
    arr->data = (Node**)realloc(arr->data, new_capacity * sizeof(Node*));
    arr->capacity = new_capacity;
}

//////////////////////////////////////////////
// 合併相鄰段 (merge_segments)
//////////////////////////////////////////////
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

//////////////////////////////////////////////
// flush_pending：刷新累積的 type 3 更新
//////////////////////////////////////////////
void flush_pending(NodeArray *arr) {
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

//////////////////////////////////////////////
// process_removals：刪除所有鑽石值 < v 的段（或部分段）
//////////////////////////////////////////////
ll process_removals(NodeArray *arr, ll v) {
    ll removed = 0;
    // 從陣列末端（最低值段）向上檢查
    while (arr->size > 0) {
        Node *node = arr->data[arr->size - 1];
        ll last = node->value + (node->count - 1) * node->diff;
        if (node->value < v) {
            // 整個段的鑽石皆 < v：刪除整段
            removed += node->count;
            total_count -= node->count;
            free(node);
            arr->size--;
        } else if (node->value >= v && last < v) {
            // 此段部分跨過 v：
            // 由於段為算術序列 (diff 不為 0)，且鑽石值從 node->value 遞減，
            // 保留前 k 顆鑽石使得其值都 >= v，刪除剩下的。
            ll d = -node->diff; // 正數
            ll k = (node->value - v) / d + 1;
            if (k > node->count) k = node->count; // safety
            removed += (node->count - k);
            total_count -= (node->count - k);
            node->count = k;
            break;
        } else {
            // 此段全部 >= v，停止
            break;
        }
    }
    return removed;
}

//////////////////////////////////////////////
// 二分搜尋：找出第一個鑽石值 < v 的位置（陣列從大到小排序）
//////////////////////////////////////////////
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

//////////////////////////////////////////////
// insert_node：插入新段，保持陣列依鑽石值遞減排序
//////////////////////////////////////////////
void insert_node(NodeArray *arr, int Ni, ll v) {
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

//////////////////////////////////////////////
// process_type_1：刷新 pending，刪除所有鑽石值 < v，再插入新段
//////////////////////////////////////////////
void process_type_1(int Ni, ll v) {
    if (pending > 0)
        flush_pending(&segArr);
    ll rem = process_removals(&segArr, v);
    printf("%lld\n", rem);
    insert_node(&segArr, Ni, v);
}

//////////////////////////////////////////////
// process_type_2：查詢鑽石值等於 p 的鑽石數
//////////////////////////////////////////////
// 輔助函數：二分搜尋，找出陣列中最後一個滿足 data[i]->value >= p 的 index
// 若找不到則返回 -1
int binary_search_candidate(NodeArray *arr, ll p) {
    int lo = 0, hi = arr->size - 1;
    int candidate = -1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (arr->data[mid]->value >= p) {
            candidate = mid;  // 更新候選 index
            lo = mid + 1;     // 嘗試找更靠後（值較小但仍 ≥ p）的段
        } else {
            hi = mid - 1;
        }
    }
    return candidate;
}

// 修改後的 process_type_2：利用 binary search 找候選段，再檢查 candidate 的前後相鄰段
void process_type_2(ll p) {
    if (pending > 0)
        flush_pending(&segArr);
    ll ans = 0;
    
    int candidate = binary_search_candidate(&segArr, p);
    // 為避免漏掉邊界情況，檢查 candidate, candidate-1 以及 candidate+1（如果存在）
    for (int i = candidate - 1; i <= candidate + 1; i++) {
        if (i < 0 || i >= segArr.size)
            continue;
        Node *node = segArr.data[i];
        if (node->diff == 0) {
            if (node->value == p)
                ans += node->count;
        } else {
            ll last = node->value + (node->count - 1) * node->diff;
            // 注意：因為段內數值由大到小排列，範圍為 [node->value, last]（node->value >= last）
            if (p <= node->value && p >= last) {
                ll diff = -node->diff; // 轉為正數
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



//////////////////////////////////////////////
// process_type_3：僅增加 pending (O(1))
//////////////////////////////////////////////
void process_type_3() {
    pending++;
}

//////////////////////////////////////////////
// main
//////////////////////////////////////////////
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
