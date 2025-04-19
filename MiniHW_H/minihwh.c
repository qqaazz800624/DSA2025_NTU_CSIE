#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

int char_to_val(char c);
void rabin_karp_hash(int k, int width, int q, int *powExp, const char *text2D, int *primeArr);
void print_primeArr(int n, int *primeArr);
void kmp_prefix_function(int m, int *Pprime, int *pi);
int kmp_matcher(int n, int m, int *Tprime, int *Pprime, int *pi, int *matches);
void check_spurious(int k, int n, int m, int matchCnt, int *matches, char *T_data, char *P_data);

int main(){

    int k, n, m, q;
    scanf("%d %d %d %d", &k, &n, &m, &q);

    char *T_data, *P_data;
    T_data = malloc((size_t)k * n);
    P_data = malloc((size_t)k * m);

    int max_nm;
    if (n > m) {
        max_nm = n;
    } else {
        max_nm = m;
    }
    char *line;
    line = malloc(max_nm + 1);

    for (int i = 0; i < k; i++) {
        scanf("%s", line);
        memcpy(T_data + (size_t)i * n, line, n);
    }

    for (int i = 0; i < k; i++) {
        scanf("%s", line);
        memcpy(P_data + (size_t)i * m, line, m);
    }

    free(line);

    const int d = 52;
    int *powExp;
    powExp = malloc((size_t)k * sizeof(int));
    powExp[0] = 1;
    for (int i = 1; i < k; i++) {
        powExp[i] = (int)((int64_t)powExp[i-1] * d % q);
    }

    int *Tprime, *Pprime;
    Tprime = malloc((size_t)n * sizeof(int));
    Pprime = malloc((size_t)m * sizeof(int));

    rabin_karp_hash(k, n, q, powExp, T_data, Tprime);
    rabin_karp_hash(k, m, q, powExp, P_data, Pprime);

    print_primeArr(n, Tprime);
    print_primeArr(m, Pprime);
    
    int *pi;
    pi = malloc((size_t)m * sizeof(int));
    kmp_prefix_function(m, Pprime, pi);

    int *matches;
    int matchCnt = 0;
    matches = malloc((size_t)(n + 1) * sizeof(int));
    matchCnt = kmp_matcher(n, m, Tprime, Pprime, pi, matches);

    if (matchCnt == 0) {
        printf("-1\n");
    } else {
        for (int i = 0; i < matchCnt; i++) {
            printf("%d", matches[i]);
            if (i + 1 < matchCnt) putchar(' ');
        }
        putchar('\n');
    }

    check_spurious(k, n, m, matchCnt, matches, T_data, P_data);

    free(T_data);
    free(P_data);
    free(powExp);
    free(Tprime);
    free(Pprime);
    free(pi);
    free(matches);
    
    return 0;
}

int char_to_val(char c){
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else {
        return c - 'a' + 26;
    }
}


void rabin_karp_hash(int k, int width, int q, int *powExp, const char *text2D, int *primeArr){
    for (int j = 0; j < width; j++) {
        int64_t h = 0;
        for (int i = 0; i < k; i++) {
            char c = text2D[i * width + j];
            int  v = char_to_val(c);
            h = (h + (int64_t)v * powExp[k-1-i]) % q;
        }
        primeArr[j] = (int)h;
    }
}


void print_primeArr(int n, int *primeArr){
    for (int j = 0; j < n; j++){
        printf("%d", primeArr[j]);
        if (j + 1 < n) {
            putchar(' ');
        }
    }
    putchar('\n');
}

void kmp_prefix_function(int m, int *Pprime, int *pi){
    pi[0] = 0;
    for (int i = 1, k = 0; i < m; i++){
        while (k > 0 && Pprime[i] != Pprime[k]){
            k = pi[k-1];
        }
        if (Pprime[i] == Pprime[k]){
            k++;
        }
        pi[i] = k;
    }
}

int kmp_matcher(int n, int m, int *Tprime, int *Pprime, int *pi, int *matches){
    int matchCnt = 0;
    for (int i = 0, q = 0; i < n; i++){
        while (q > 0 && Pprime[q] != Tprime[i]){
            q = pi[q-1];
        }
        if (Pprime[q] == Tprime[i]){
            q++;
        }
        if (q == m){
            matches[matchCnt++] = i - m + 1;
            q = pi[q-1];
        }
    }
    return matchCnt;
}

void check_spurious(int k, int n, int m, int matchCnt, int *matches, char *T_data, char *P_data){
    int *spurious;
    spurious = malloc((matchCnt + 1) * sizeof(int));
    int spuriousCnt = 0;
    for (int idx = 0; idx < matchCnt; idx++){
        int s = matches[idx];
        bool isSpurious = false;
        for (int i = 0; i < k; i++){
            if (memcmp(T_data + (size_t)i*n + s,
                       P_data + (size_t)i*m,
                       (size_t)m) != 0)
            {
                isSpurious = true;
                break;
            }
        }
        if (isSpurious){
            spurious[spuriousCnt++] = s;
        }
    }

    if (spuriousCnt == 0){
        printf("-1\n");
    } else {
        for (int i = 0; i < spuriousCnt; i++){
            printf("%d", spurious[i]);
            if (i + 1 < spuriousCnt) {
                putchar(' ');
            }
        }
        putchar('\n');
    }
    free(spurious);
}