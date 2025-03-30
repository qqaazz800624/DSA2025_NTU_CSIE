#include <stdio.h>
#include <stdlib.h>

#define MAX_HEAP_SIZE 1000000

typedef struct {
    int job_id;
    int priority;
} Job;

Job heap[MAX_HEAP_SIZE];
int heapSize = 0;

void insert(Job newJob);
void remove_max();
void remove_min();
int getLevel(int i);
void swap(int i, int j);
void bubble_up_max(int i);
void bubble_up_min(int i);
void trickle_down_min(int i);
void trickle_down_max(int i);

int main(){

    int N;
    scanf("%d", &N);

    for (int i = 0; i < N; i++){
        int op;
        scanf("%d", &op);
        if (op == 1){
            Job newJob;
            scanf("%d %d", &newJob.job_id, &newJob.priority);
            insert(newJob);
            printf("%d jobs waiting\n", heapSize);
        } else if (op == 2){
            remove_max();
        } else if (op == 3){
            remove_min();
        }
    }

    return 0;
}

int getLevel(int i){
    int level = 0;
    while (i > 1) {
        i /= 2;
        level++;
    }
    return level;
}

void swap(int i, int j){
    Job temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

void insert(Job newJob){
    heapSize++;
    heap[heapSize] = newJob; // Add new job to the end of the heap

    if (heapSize == 1) {
        return;
    }

    int i = heapSize;
    int parent = i / 2; 
    int lvl = getLevel(i);
    if (lvl % 2 == 0){ // even level: min level
        if (heap[i].priority > heap[parent].priority) {
            swap(i, parent);
            bubble_up_max(parent); 
        } else {
            bubble_up_min(i); 
        }
    } else { // odd level: max level
        if (heap[i].priority < heap[parent].priority){
            swap(i, parent);
            bubble_up_min(parent);
        } else {
            bubble_up_max(i);
        }
    }
}

void bubble_up_max(int i){
    while (i > 3) {
        int grandparent = i / 4;
        if (heap[i].priority > heap[grandparent].priority) {
            swap(i, grandparent);
            i = grandparent;
        } else {
            break;
        }
    }
}

void bubble_up_min(int i){
    while (i > 3) {
        int grandparent = i / 4;
        if (heap[i].priority < heap[grandparent].priority) {
            swap(i, grandparent);
            i = grandparent;
        } else {
            break;
        }
    }
}

void trickle_down_min(int i){
    while (1) {
        int m = i; 
        int left = 2 * i, right = left + 1;
        
        if(left <= heapSize && heap[left].priority < heap[m].priority){
            m = left;
        }
        if(right <= heapSize && heap[right].priority < heap[m].priority){
            m = right;
        }
        
        for (int k = 0; k < 4; k++) {
            int j = 4 * i + k;
            if(j <= heapSize && heap[j].priority < heap[m].priority){
                m = j;
            }
        }

        if(m == i){ // if no smaller descendant is found
            break;
        }
        
        if(m >= 4 * i && m <= 4 * i + 3) {
            swap(i, m); // swap with a grandchild
            int parent = m / 2;
            if(heap[m].priority > heap[parent].priority){ // if the moved element is greater than its parent
                swap(m, parent);
            }
            i = m;
        } else {  
            swap(i, m); // swap with a child
            break;
        }
    }
}

void trickle_down_max(int i) {
    while (1) {
        int m = i; 
        int left = 2 * i, right = left + 1;

        if(left <= heapSize && heap[left].priority > heap[m].priority){
            m = left;
        }

        if(right <= heapSize && heap[right].priority > heap[m].priority){
            m = right;
        }
        
        for (int k = 0; k < 4; k++) {
            int j = 4 * i + k;
            if(j <= heapSize && heap[j].priority > heap[m].priority)
                m = j;
        }

        if(m == i)
            break;
        
        if(m >= 4 * i && m <= 4 * i + 3) {
            swap(i, m);
            int parent = m / 2;
            if(heap[m].priority < heap[parent].priority){
                swap(m, parent);
            }
            i = m;
        } else {  
            swap(i, m);
            break;
        }
    }
}

void remove_max(){
    if(heapSize == 0) {
        printf("no job in queue\n");
        return;
    }

    int maxIndex;

    if(heapSize == 1){
        maxIndex = 1;
    } else if(heapSize == 2){
        maxIndex = 2;
    } else {
        if (heap[2].priority > heap[3].priority){
            maxIndex = 2;
        } else {
            maxIndex = 3;
        }
    }

    Job removed = heap[maxIndex];
    Job x = heap[heapSize];  // take the last job as the candidate to fill the gap
    heapSize--;

    if(maxIndex <= heapSize + 1) {
        heap[maxIndex] = x;
        if(maxIndex > 1) {
            int lvl = getLevel(maxIndex);
            int parent = maxIndex / 2;

            if ((lvl % 2 == 0 && heap[maxIndex].priority > heap[parent].priority) ||
                (lvl % 2 == 1 && heap[maxIndex].priority < heap[parent].priority)) {
                if(lvl % 2 == 0){
                    bubble_up_max(maxIndex);
                } else {
                    bubble_up_min(maxIndex);
                }
            } else {
                if(lvl % 2 == 0){
                    trickle_down_min(maxIndex);
                } else {
                    trickle_down_max(maxIndex);
                }
            }
        } else { 
            if(heapSize > 0){
                trickle_down_min(1);
            }
        }
    }
    printf("job %d with priority %d completed\n", removed.job_id, removed.priority);
}

void remove_min(){
    if(heapSize == 0) {
        printf("no job in queue\n");
        return;
    }
    Job removed = heap[1]; // the smallest priority job is always at index 1: the root
    Job x = heap[heapSize];
    heapSize--;
    if(heapSize > 0) {
        heap[1] = x;
        trickle_down_min(1);
    }
    printf("job %d with priority %d dropped\n", removed.job_id, removed.priority);
}