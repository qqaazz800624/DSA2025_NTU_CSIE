#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int B;
int swap_count = 0;
bool finished = false;


void safe_swap(int *a, int *b);
int Partition(int a[], int l, int r);
void QUICKSORT(int a[], int l, int r);

int main(){

    int N;
    scanf("%d %d", &N, &B);
    int *a = (int *)malloc(N * sizeof(int));

    for (int i = 0; i < N; i++){
        scanf("%d", &a[i]);
    }
    QUICKSORT(a, 0, N - 1);
    bool sorted = true;
    for (int i = 0; i < N - 1; i++){
        if (a[i] > a[i + 1]){
            sorted = false;
            break;
        }
    }

    printf("The array is");
    for (int i = 0; i < N; i++){
        printf(" %d", a[i]);
    }
    if (sorted){
        printf(" after %d swaps.\n", swap_count);
    } else {
        printf(" after %d swaps.\n", B);
    }

    free(a);

    return 0;
}

void safe_swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
    swap_count++;
    if (swap_count == B){
        finished = true;
    }
}

int Partition(int a[], int l, int r){
    int pivot = a[l];
    int i = l + 1;
    int j = r;

    while (i < j && !finished){
        while (i <= r && a[i] <= pivot){
            i++;
        }
        while (j >= l && a[j] > pivot){
            j--;
        }
        if (i >= j){
            break;
        }
        if (!finished){
            safe_swap(&a[i], &a[j]);
            if (finished){
                return j;
            }
        }
    }
    if (l < j && a[l] > a[j] && !finished){
        safe_swap(&a[l], &a[j]);
    }
    return j;
}

void QUICKSORT(int a[], int l, int r){
    if (l < r && !finished){
        int m = Partition(a, l, r);
        if (finished){
            return;
        }
        QUICKSORT(a, l, m - 1);
        if (finished){
            return;
        }
        QUICKSORT(a, m + 1, r);
    }
}