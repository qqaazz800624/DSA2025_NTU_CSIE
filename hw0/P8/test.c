#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main(){
    int n, m;
    scanf("%d %d", &n, &m);

    // Maximum n = 10, name length <= 64, so we use fixed arrays.
    char names[11][65];
    int boards[11][256][256]; // Maximu m = 256
    bool marks[11][256][256] = {0}; // Marks for each board cell, initially all false.

    int i, r, c;
    for (i = 0; i < n; i++) {
        scanf("%s", names[i]);
        for (r = 0; r < m; r++) {
            for (c = 0; c < m; c++) {
                scanf("%d", &boards[i][r][c]);
            }
        }
    }

    // Use dynamic array to store calls
    int totalCalls = m * m;
    int *calls;
    calls = malloc(totalCalls * sizeof(int));
    if (!calls) return 1;
    for (i = 0; i < totalCalls; i++) {
        scanf("%d", &calls[i]);
    }

    bool winners[11] = {0}; // Maximum n = 10 players
    bool foundWin = false;  // Flag to indicate if a player wins --> time to stop
    int winningCall = -1;   // Record the winning call number, initialize to -1

    int callIdx, number;
    for (callIdx = 0; callIdx < totalCalls; callIdx++){
        number = calls[callIdx];
        for (i = 0; i < n; i++){
            for (r = 0; r < m; r++){
                for (c = 0; c < m; c++){
                    if (boards[i][r][c] == number){
                        marks[i][r][c] = true;
                    }
                }
            }
        }

        // check each board, from rows, columns, main diagonal, anti-diagonal
        for (i=0; i<n; i++){
            bool win = false;
            // check rows
            for (r=0; r<m && !win; r++){
                bool rowWin = true;
                for (c=0; c<m; c++){
                    if (!marks[i][r][c]){
                        rowWin = false;
                        break;
                    }
                }
                if (rowWin){
                    win = true;
                }
            }
            // check columns
            for (c=0; c<m && !win; c++){
                bool colWin = true;
                for (r=0; r<m; r++){
                    if (!marks[i][r][c]){
                        colWin = false;
                        break;
                    }
                }
                if (colWin){
                    win = true;
                }
            }
            // check main diagonal
            if (!win){
                bool diagWin = true;
                int d;
                for (d=0; d<m; d++){
                    if (!marks[i][d][d]){
                        diagWin = false;
                        break;
                    }
                }
                if (diagWin){
                    win = true;
                }
            }
            // check anti-diagonal
            if (!win){
                bool antiDiagWin = true;
                int d;
                for (d=0; d<m; d++){
                    if (!marks[i][d][m-1-d]){
                        antiDiagWin = false;
                        break;
                    }
                }
                if (antiDiagWin){
                    win = true;
                }
            }
            if (win){
                winners[i] = true;
                foundWin = true;
            }
        }

        if (foundWin){
            winningCall = number;
            break;
        }
    }

    // release memory
    free(calls);

    if (winningCall != -1){
        printf("%d", winningCall);
        for (i=0; i<n; i++){
            if (winners[i]){
                printf(" %s", names[i]);
            }
        }
        printf("\n");
    }

    return 0;
}