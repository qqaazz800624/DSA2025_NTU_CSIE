#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void PrintStatus(int key, int l, int r);
void Bin_Search(int a[], int key, int l, int r);

int main(){

    int N, key;
    scanf("%d %d", &N, &key);

    int a[N];
    int i;
    for(i = 0; i < N; i++){
        scanf("%d", &a[i]);
    }

    int l = 0;
    int r = N - 1;

    Bin_Search(a, key, l, r);

    return 0;
}

void PrintStatus(int key, int l, int r){
    printf("Searching %d in range [%d, %d].\n", key, l+1, r+1);
}

void Bin_Search(int a[], int key, int l, int r){
    bool found = false;
    while (l <= r){
        PrintStatus(key, l, r);
        int m = (l + r) / 2;
        if (a[m] == key){
            printf("Found at index %d.\n", m+1);
            found = true;
            break;
        } else if (a[m] < key){
            l = m + 1;
        } else {
            r = m - 1;
        }
    }
    if (!found){
        PrintStatus(key, l, r);
        printf("Not found.\n");
    }
}
