#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int backMoves = 0;

void printArray(int a[], int N, int count);
void Insert_Bigger(int a[], int m, int N, int B);
void Insertion_Sort(int a[], int N, int B);

int main(){
    int N, B;
    scanf("%d %d", &N, &B);

    int a[N];
    int i;
    for(i = 0; i < N; i++){
        scanf("%d", &a[i]);
    }
    Insertion_Sort(a, N, B);
    return 0;
}

void Insert_Bigger(int a[], int m, int N, int B) {
    int data = a[m];
    int i = m - 1;
    while(i >= 0 && a[i] < data && backMoves < B){
        a[i + 1] = a[i];
        i = i - 1;
        backMoves++;
    }
    a[i + 1] = data;
}

void Insertion_Sort(int a[], int N, int B){
    int i;
    bool finishedEarly = false;
    for(i = 1; i < N; i++){
        Insert_Bigger(a, i, N, B);
        if(backMoves == B){
            finishedEarly = true;
            break;
        }
    }
    if (finishedEarly){
        printArray(a, N, B);
    } else {
        printArray(a, N, backMoves);
    }
}

void printArray(int a[], int N, int count) {
    printf("The array is");
    int i;
    for (i = 0; i < N; i++) {
        printf(" %d", a[i]);
    }
    printf(" after %d back-moves.\n", count);
}