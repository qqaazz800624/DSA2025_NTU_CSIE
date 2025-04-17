#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compute_prefix_function(const char *p, int n, int *pi) {
    pi[0] = 0;
    int k = 0;
    for (int q = 1; q < n; q++){
        while (k > 0 && p[k] != p[q]) {
            k = pi[k-1];
        }
        if (p[k] == p[q]) {
            k++;
        }
        pi[q] = k;
    }
}

int main(void) {
    int n;
    scanf("%d", &n);
    char *S = malloc((n + 1) * sizeof(char));

    // 3. Read the pattern string (no spaces in it)
    if (scanf("%s", S) != 1) {
        fprintf(stderr, "Failed to read string\n");
        free(S);
        return 1;
    }
    // Optional check: ensure the read string length matches n
    if ((int)strlen(S) != n) {
        fprintf(stderr, "Warning: expected length %d, got %zu\n", n, strlen(S));
        // you can choose to exit here, or continue
    }

    // 4. Allocate and compute π
    int *pi = malloc(n * sizeof(int));
    if (!pi) {
        perror("malloc");
        free(S);
        return 1;
    }
    compute_prefix_function(S, n, pi);

    // 5. Print the π array
    printf("π: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", pi[i]);
    }
    printf("\n");

    // 6. Cleanup
    free(S);
    free(pi);
    return 0;
}
