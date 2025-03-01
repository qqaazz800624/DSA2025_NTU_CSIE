#include <stdio.h>
#include <stdlib.h>

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

    if (backMoves < B) {
        printArray(a, N, backMoves);
    } else {
        printArray(a, N, B);
    }

    return 0;
}

void Insert_Bigger(int a[], int m, int N, int B) {
    int data = a[m];
    int j = m - 1;
    while(j >= 0 && a[j] < data && backMoves < B){
        a[j + 1] = a[j];
        j--;
        backMoves++;
    }
    a[j + 1] = data;
}

void Insertion_Sort(int a[], int N, int B){
    for(int i = 1; i < N; i++){
        Insert_Bigger(a, i, N, B);
        if(backMoves == B){
            break;
        }
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