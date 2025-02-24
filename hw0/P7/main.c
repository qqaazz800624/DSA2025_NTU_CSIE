#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 9

int N[SIZE][SIZE];
int M[SIZE][SIZE];
bool solved = false;

int max_int(int a, int b){
    if (a > b){
        return a;
    } else {
        return b;
    }
}

int min_int(int a, int b){
    if (a < b){
        return a;
    } else {
        return b;
    }
}

// check the row r, for each cell (r, j), the sum of mines in its neighborhood 
// equals N[r][j]
bool check_row(int r){
    int j;
    for (j = 0; j < SIZE; j++){
        int sum = 0;
        int r_start = max_int(0, r - 1);
        int r_end = min_int(SIZE - 1, r + 1);
        int c_start = max_int(0, j - 1);
        int c_end = min_int(SIZE - 1, j + 1);
        int i, k;
        for (i = r_start; i <= r_end; i++){
            for (k = c_start; k <= c_end; k++){
                sum += M[i][k];
            }
        }
        if (sum != N[r][j]){
            return false;
        }
    }
    return true;
}

void solve(int idx){
    if (idx == SIZE * SIZE){
        if (!check_row(SIZE - 1) || !check_row(SIZE - 2)){
            return;
        }
        solved = true;
        return;
    }
    int i, j;
    i = idx / SIZE; // check which row the idx is in
    j = idx % SIZE; // check which column the idx is in
    int val;
    for (val = 0; val <= 1; val++){
        M[i][j] = val;
        if (j == SIZE - 1){
            if (i >= 1 && !check_row(i - 1)){
                continue;
            }
            if (i >= 2 && !check_row(i - 2)){
                continue;
            }
        }
        solve(idx + 1);
        if (solved){
            return;
        }
    }
}

int main(){

    // read the 9x9 grid N(the neighbor counts)
    int i, j;
    for (i = 0; i < SIZE; i++){
        for (j = 0; j < SIZE; j++){
            scanf("%d", &N[i][j]);
        }
    }

    solve(0);

    if (!solved) {
        printf("no solution\n");
    } else {
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                printf("%d", M[i][j]);
                if (j < SIZE - 1)
                    printf(" ");
            }
            printf("\n");
        }
    }

    return 0;
}