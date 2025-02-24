#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_N 10        // maximum number of players
#define MAX_M 256       // maximum board dimension
#define MAX_CALLS (MAX_M * MAX_M)

int main(void) {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) return 1;
    
    // Arrays to store names and board mapping.
    char names[MAX_N][65];
    // pos[i][num][0] and pos[i][num][1] store the row and col for board i for number "num".
    // Since board numbers range from 1 to m*m, we allocate size (m*m + 1).
    int pos[MAX_N][MAX_M * MAX_M + 1][2];
    
    // Read each board.
    for (int i = 0; i < n; i++) {
        scanf("%s", names[i]);
        for (int r = 0; r < m; r++) {
            for (int c = 0; c < m; c++) {
                int num;
                scanf("%d", &num);
                pos[i][num][0] = r;
                pos[i][num][1] = c;
            }
        }
    }
    
    // Read the sequence of calls (total m*m numbers)
    int totalCalls = m * m;
    int *calls = malloc(totalCalls * sizeof(int));
    if (!calls) return 1;
    for (int i = 0; i < totalCalls; i++) {
        scanf("%d", &calls[i]);
    }
    
    // For each board, we maintain counters for rows, columns, and both diagonals.
    int rowCount[MAX_N][MAX_M] = {0};
    int colCount[MAX_N][MAX_M] = {0};
    int diagCount[MAX_N] = {0};
    int antiDiagCount[MAX_N] = {0};
    
    // Array to mark winners.
    bool winners[MAX_N] = {false};
    bool foundWin = false;
    int winningCall = -1;
    
    // Process each called number.
    for (int callIdx = 0; callIdx < totalCalls; callIdx++){
        int number = calls[callIdx];
        for (int i = 0; i < n; i++){
            // Get the position of "number" on board i.
            int r = pos[i][number][0];
            int c = pos[i][number][1];
            
            // Update counters.
            rowCount[i][r]++;
            colCount[i][c]++;
            if (r == c) diagCount[i]++;
            if (r + c == m - 1) antiDiagCount[i]++;
            
            // Check if this board has achieved bingo.
            if (!winners[i] && (rowCount[i][r] == m || colCount[i][c] == m ||
                                diagCount[i] == m || antiDiagCount[i] == m)) {
                winners[i] = true;
                foundWin = true;
            }
        }
        if (foundWin) {
            winningCall = number;
            break;
        }
    }
    free(calls);
    
    // Output: winning call number followed by winners' names (in input order).
    if (winningCall == -1) {
        printf("no solution\n");
    } else {
        printf("%d", winningCall);
        for (int i = 0; i < n; i++){
            if (winners[i]){
                printf(" %s", names[i]);
            }
        }
        printf("\n");
    }
    
    return 0;
}
