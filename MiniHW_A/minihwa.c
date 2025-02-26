#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int comparison_count = 0;

void print_partial_array(int a[], int N, int used_budget, bool budget_exhausted);
int Get_Min_Index(int a[], int l, int r, int *B, int N);
void Selection_Sort(int a[], int N, int B);


int main(){

    int N, B;
    scanf("%d %d", &N, &B);

    int a[N];
    int i;
    for (i = 0; i < N; i++){
        scanf("%d", &a[i]);
    }

    Selection_Sort(a, N, B);

    return 0;
}

void print_partial_array(int a[], int N, int used_budget, bool budget_exhausted){
    printf("The array is");
    int i;
    for (i = 0; i < N; i++){
        printf(" %d", a[i]);
    }
    if (budget_exhausted){
        printf(" after %d comparisons.\n", used_budget);
    }
    else{
        printf(" after %d comparisons.\n", comparison_count);
    }
}

int Get_Min_Index(int a[], int l, int r, int *B, int N){
    int m = l;
    int i;
    for (i = l+1; i <= r; i++){
        if (comparison_count == *B){
            print_partial_array(a, N, *B, true);
            exit(0);
        }
        comparison_count++;
        if (a[i] < a[m]){
            m = i;
        }
    }
    return m;
}

void Selection_Sort(int a[], int N, int B){
    int i, j, m;
    for (i = 0; i < N - 1; i++){
        m = Get_Min_Index(a, i, N-1, &B, N);
        if (i != m){
            int temp = a[i];
            a[i] = a[m];
            a[m] = temp;
        }
    }
    print_partial_array(a, N, B, false);
}