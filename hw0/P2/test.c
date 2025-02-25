#include <stdio.h>
#include <stdlib.h>

#define SHELF_CAPACITY 8

typedef unsigned long long UINT64;

UINT64 shelfState = 0;
int shelf[SHELF_CAPACITY] = {0};

void updateShelfState();
void readBook(int bookId);
void printShelfContents();

int main(){

    int N, op, BookId, format, i;

    scanf("%d", &N);
    for (i = 0; i < N; i++){
        scanf("%d", &op);
        
        if (op == 1){
            scanf("%d", &BookId);
            readBook(BookId);
        } else if (op == 2){
            scanf("%d", &format);
            if (format == 1){
                printf("%llu\n", shelfState);
            } else if (format == 2){
                printShelfContents();
            }
        }
    }

    return 0;
}

void updateShelfState(){
    shelfState = 0;
    int i;
    for (i = 0; i < SHELF_CAPACITY; i++){
        shelfState = (shelfState << 8) | (UINT64)shelf[i];
    }
}

void readBook(int bookId){
    int found = -1;
    int i; 
    for (i = 0; i < SHELF_CAPACITY; i++){
        if (shelf[i] == bookId){
            found = i;
            break;
        }
    }
    if (found != -1){
        for (i = found; i < SHELF_CAPACITY - 1; i++){
            shelf[i] = shelf[i + 1];
        }
        shelf[SHELF_CAPACITY - 1] = bookId;
    } else {
        for (i = 0; i < SHELF_CAPACITY - 1; i++){
            shelf[i] = shelf[i + 1];
        }
        shelf[SHELF_CAPACITY - 1] = bookId;
    }
    updateShelfState();
}

void printShelfContents() {
    for (int i = 0; i < SHELF_CAPACITY; i++) {
        printf("%d ", shelf[i]);
    }
    printf("\n");
}