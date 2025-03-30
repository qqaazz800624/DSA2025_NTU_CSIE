#include <stdio.h>
#include <stdlib.h>

// Define the maximum number of operations plus one extra (1-indexed array)
#define MAX_HEAP_SIZE 1000001

// Structure to store a job (job_id and priority)
typedef struct {
    int job_id;
    int priority;
} Job;

// Global heap array and current heap size.
Job heap[MAX_HEAP_SIZE];
int heapSize = 0;

// Utility: Compute the level (0-indexed) of a node at index i.
// Level = floor(log2(i))
int getLevel(int i) {
    int level = 0;
    while(i > 1) {
        i /= 2;
        level++;
    }
    return level;
}

// Utility: Swap two elements in the heap.
void swap(int i, int j) {
    Job temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

// Bubble-up procedure for a node on a min-level.
void bubble_up_min(int i) {
    // While the node has a grandparent (i > 3 guarantees that)
    while(i > 3) {
        int grandparent = i / 4;  // because parent's parent = i/2/2 = i/4 (integer division)
        if(heap[i].priority < heap[grandparent].priority) {
            swap(i, grandparent);
            i = grandparent;
        } else {
            break;
        }
    }
}

// Bubble-up procedure for a node on a max-level.
void bubble_up_max(int i) {
    while(i > 3) {
        int grandparent = i / 4;
        if(heap[i].priority > heap[grandparent].priority) {
            swap(i, grandparent);
            i = grandparent;
        } else {
            break;
        }
    }
}

// Trickle-down procedure for a node on a min-level.
void trickle_down_min(int i) {
    while (1) {
        int m = i; // m will be the index of the smallest among children and grandchildren
        int left = 2 * i, right = left + 1;
        // Check children
        if(left <= heapSize && heap[left].priority < heap[m].priority)
            m = left;
        if(right <= heapSize && heap[right].priority < heap[m].priority)
            m = right;
        // Check grandchildren (four possible grandchildren)
        for (int k = 0; k < 4; k++) {
            int j = 4 * i + k;
            if(j <= heapSize && heap[j].priority < heap[m].priority)
                m = j;
        }
        if(m == i)
            break;  // no smaller descendant found
        // If m is a grandchild, then do a two–step swap.
        if(m >= 4 * i && m <= 4 * i + 3) {
            swap(i, m);
            int parent = m / 2;
            // After swapping, if the moved element is greater than its parent, swap with the parent.
            if(heap[m].priority > heap[parent].priority)
                swap(m, parent);
            i = m;
        } else {  // m is a child – one swap is enough.
            swap(i, m);
            break;
        }
    }
}

// Trickle-down procedure for a node on a max-level.
void trickle_down_max(int i) {
    while (1) {
        int m = i; // m will be the index of the largest among children and grandchildren
        int left = 2 * i, right = left + 1;
        if(left <= heapSize && heap[left].priority > heap[m].priority)
            m = left;
        if(right <= heapSize && heap[right].priority > heap[m].priority)
            m = right;
        // Check grandchildren (four possible grandchildren)
        for (int k = 0; k < 4; k++) {
            int j = 4 * i + k;
            if(j <= heapSize && heap[j].priority > heap[m].priority)
                m = j;
        }
        if(m == i)
            break;
        // If m is a grandchild:
        if(m >= 4 * i && m <= 4 * i + 3) {
            swap(i, m);
            int parent = m / 2;
            if(heap[m].priority < heap[parent].priority)
                swap(m, parent);
            i = m;
        } else {  // m is a child.
            swap(i, m);
            break;
        }
    }
}

// Insert a new job into the min-max heap.
void insert(Job newJob) {
    heapSize++;
    heap[heapSize] = newJob; // Insert at the end of the heap.
    if(heapSize == 1)
        return;

    int i = heapSize;
    int parent = i / 2;
    int lvl = getLevel(i);
    if(lvl % 2 == 0) {  // Even level: min-level
        if(heap[i].priority > heap[parent].priority) {
            swap(i, parent);
            bubble_up_max(parent);
        } else {
            bubble_up_min(i);
        }
    } else {  // Odd level: max-level
        if(heap[i].priority < heap[parent].priority) {
            swap(i, parent);
            bubble_up_min(parent);
        } else {
            bubble_up_max(i);
        }
    }
}

// Remove the job with the maximum priority (complete operation).
void remove_max() {
    if(heapSize == 0) {
        printf("no job in queue\n");
        return;
    }
    int maxIndex;
    if(heapSize == 1)
        maxIndex = 1;
    else if(heapSize == 2)
        maxIndex = 2;
    else
        maxIndex = (heap[2].priority > heap[3].priority) ? 2 : 3;

    Job removed = heap[maxIndex];
    Job x = heap[heapSize];  // Last element to be re–inserted.
    heapSize--;
    if(maxIndex <= heapSize + 1) {
        heap[maxIndex] = x;
        // Decide whether to bubble up or trickle down.
        if(maxIndex > 1) {
            int parent = maxIndex / 2;
            int lvl = getLevel(maxIndex);
            if ((lvl % 2 == 0 && heap[maxIndex].priority > heap[parent].priority) ||
                (lvl % 2 == 1 && heap[maxIndex].priority < heap[parent].priority)) {
                if(lvl % 2 == 0)
                    bubble_up_max(maxIndex);
                else
                    bubble_up_min(maxIndex);
            } else {
                if(lvl % 2 == 0)
                    trickle_down_min(maxIndex);
                else
                    trickle_down_max(maxIndex);
            }
        } else { // maxIndex == 1 (root)
            if(heapSize > 0)
                trickle_down_min(1);
        }
    }
    printf("job %d with priority %d completed\n", removed.job_id, removed.priority);
}

// Remove the job with the minimum priority (drop operation).
void remove_min() {
    if(heapSize == 0) {
        printf("no job in queue\n");
        return;
    }
    Job removed = heap[1];
    Job x = heap[heapSize];
    heapSize--;
    if(heapSize > 0) {
        heap[1] = x;
        trickle_down_min(1);
    }
    printf("job %d with priority %d dropped\n", removed.job_id, removed.priority);
}

int main(){
    int N;
    // Read total number of operations.
    if(scanf("%d", &N) != 1) return 1;
    
    for (int i = 0; i < N; i++){
        int op;
        scanf("%d", &op);
        if(op == 1) {
            // Add operation: format "1 job_id priority"
            Job newJob;
            scanf("%d %d", &newJob.job_id, &newJob.priority);
            insert(newJob);
            printf("%d jobs waiting\n", heapSize);
        } else if(op == 2) {
            // Complete operation: remove the job with the highest priority.
            remove_max();
        } else if(op == 3) {
            // Drop operation: remove the job with the lowest priority.
            remove_min();
        }
    }
    return 0;
}
