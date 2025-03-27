#include <stdio.h>
#include <limits.h>

#define INF INT_MAX

void heapIncreaseKey(int heap[], int i, int key);
void minHeapInsert(int heap[], int *heapSize, int key);

int main(){

    int N;
    scanf("%d", &N);

    int heap[N + 1];
    int heapSize = 0;

    for (int i = 0; i < N; i++){
        int key;
        scanf("%d", &key);
        minHeapInsert(heap, &heapSize, key);
    }

    for (int i = 1; i <= heapSize; i++){
        printf("%d ", heap[i]);
    }
    printf("\n");

    return 0;
}

void heapIncreaseKey(int heap[], int i, int key){
    heap[i] = key;
    while (i > 1 && heap[i / 2] > heap[i]){
        int temp = heap[i];
        heap[i] = heap[i / 2];
        heap[i / 2] = temp;
        i = i / 2;
    }
}

void minHeapInsert(int heap[], int *heapSize, int key){
    (*heapSize)++;
    heap[*heapSize] = INF;
    heapIncreaseKey(heap, *heapSize, key);
}