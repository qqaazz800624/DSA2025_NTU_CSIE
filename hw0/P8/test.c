#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 10        // maximum number of players
#define MAX_M 256       // maximum board dimension
#define MAX_CALLS (MAX_M * MAX_M)

int main(){
    int n, m;
    scanf("%d %d", &n, &m);

    char names[MAX_N][65];
    int pos[MAX_N][MAX_M * MAX_M + 1][2];
    int i, r, c;

    for (i = 0; i < n; i++){
        scanf("%s", names[i]);
        for (r = 0; r < m; r++){
            for (c = 0; c < m; c++){
                int num;
                scanf("%d", &num);
                pos[i][num][0] = r;
                pos[i][num][1] = c;
            }
        }
    }

    int totalCalls = m * m;
    int *calls;
    calls = malloc(totalCalls * sizeof(int));
    if (!calls) return 1;

    for (i = 0; i < totalCalls; i++){
        scanf("%d", &calls[i]);
    }

    int rowCount[MAX_N][MAX_M] = {0};
    int colCount[MAX_N][MAX_M] = {0};
    int diagCount[MAX_N] = {0};
    int antiDiagCount[MAX_N] = {0};

    bool winners[MAX_N] = {false};
    bool foundWin = false;
    int winningCall = -1;

    int callIdx, number;
    for (callIdx = 0; callIdx < totalCalls; callIdx++){
        number = calls[callIdx];
        for (i = 0; i < n; i++){
            int r = pos[i][number][0];
            int c = pos[i][number][1];

            rowCount[i][r]++;
            colCount[i][c]++;

            if (r == c){
                diagCount[i]++;
            }

            if (r + c == m - 1){
                antiDiagCount[i]++;
            }

            if (!winners[i] && (rowCount[i][r] == m || colCount[i][c] == m ||
                                diagCount[i] == m || antiDiagCount[i] == m)){
                winners[i] = true;
                foundWin = true;
            }
        }
        if (foundWin){
            winningCall = number;
            break;
        }
    }
    free(calls);

    if (winningCall != -1){
        printf("%d", winningCall);
        for (i = 0; i < n; i++){
            if (winners[i]){
                printf(" %s", names[i]);
            }
        }
        printf("\n");
    }

    return 0;
}