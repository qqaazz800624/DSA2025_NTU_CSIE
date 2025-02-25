#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// We assume n ≤ 20 so that 1<<n is at most about 1 million.
int main(void) {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2)
        return 1;
    
    // Read the m friendship pairs.
    // Maximum m is n(n-1)/2, so we use an array large enough.
    int friendPairs[210][2]; // a bit more than needed
    for (int i = 0; i < m; i++){
        scanf("%d %d", &friendPairs[i][0], &friendPairs[i][1]);
    }
    
    int bestMask = -1;
    int bestSize = n + 1; // initial "infinite" size
    int total = 1 << n;
    
    // Iterate over all possible subsets represented by bitmask [0, 1<<n)
    for (int mask = 0; mask < total; mask++){
        int count = __builtin_popcount(mask);
        // Skip if already larger than current best.
        if (count > bestSize) continue;
        
        bool valid = true;
        // For each friendship pair, check that at least one endpoint is included.
        for (int i = 0; i < m; i++){
            int u = friendPairs[i][0];
            int v = friendPairs[i][1];
            // If neither u nor v is in the subset, then this is not a cover.
            if (((mask >> u) & 1) == 0 && ((mask >> v) & 1) == 0) {
                valid = false;
                break;
            }
        }
        if (!valid)
            continue;
        
        // If valid, then we have a candidate cover.
        if (count < bestSize) {
            bestSize = count;
            bestMask = mask;
        } else if (count == bestSize) {
            // If candidate has the same size as the best found so far,
            // choose the lexicographically smallest cover.
            // Lexicographical order means that if you list the indices (in ascending order)
            // from the subset, the one with the smallest number at the first differing position is chosen.
            bool candidateBetter = false;
            for (int i = 0; i < n; i++){
                int bitCandidate = (mask >> i) & 1;
                int bitBest = (bestMask >> i) & 1;
                if (bitCandidate != bitBest) {
                    // If candidate has a '1' at a lower index where bestMask has '0',
                    // then candidate's sorted list starts with that index, which is lexicographically smaller.
                    if (bitCandidate == 1 && bitBest == 0) {
                        candidateBetter = true;
                    }
                    break;
                }
            }
            if (candidateBetter) {
                bestMask = mask;
            }
        }
    }
    
    // Output the indices in the best cover (in increasing order), one per line.
    // (Indices are 0-indexed.)
    for (int i = 0; i < n; i++){
        if ((bestMask >> i) & 1)
            printf("%d\n", i);
    }
    
    return 0;
}
