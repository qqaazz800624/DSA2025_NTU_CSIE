#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(){

    int n, m;
    scanf("%d %d", &n, &m);

    int friendPairs[200][2];
    int i;
    for(i = 0; i < m; i++){
        scanf("%d %d", &friendPairs[i][0], &friendPairs[i][1]);
    }

    int bestMask = -1;
    int bestSize = n + 1;
    int total = 1 << n;

    int mask;
    for (mask = 0; mask < total; mask++){
        int count;
        count = __builtin_popcount(mask);
        if (count > bestSize){
            continue;
        }

        bool valid = true;
        for(i = 0; i < m; i++){
            int u = friendPairs[i][0];
            int v = friendPairs[i][1];
            if (((mask >> u) & 1) == 0 && ((mask >> v) & 1) == 0){
                valid = false;
                break;
            }
        }
        if (!valid){
            continue;
        }

        if (count < bestSize){
            bestSize = count;
            bestMask = mask;
        } else if (count == bestSize){
            bool candidateBetter = false;
            for (i = 0; i < n; i++){
                int bitCandidate = (mask >> i) & i;
                int bitBest = (bestMask >> i) & i;
                if (bitCandidate != bitBest){
                    if (bitCandidate == 1 && bitBest == 0){
                        candidateBetter = true;
                    }
                }
                break;
            }
            if (candidateBetter){
                bestMask = mask;
            }
        }
    }

    for (i = 0; i < n; i++){
        if ((bestMask >> i) & 1){
            printf("%d\n", i);
        }
    }

    return 0;
}