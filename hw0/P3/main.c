#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_FOOD 110
#define MAX_ING 11      // each food can have up to 10 ingredients
#define MAX_NAME 65
#define MAX_QUERY 11000

typedef struct {
    char name[MAX_NAME];
    int count;
    char ingredients[MAX_ING][MAX_NAME];
} Food;

int comparestr(const void *a, const void *b){
    const char *s1 = *(const char **)a;
    const char *s2 = *(const char **)b;
    return strcmp(s1, s2);
}

int main(){

    int n;
    scanf("%d", &n);

    Food foods[MAX_FOOD];
    int i, j;
    for (i = 0; i < n; i++){
        scanf("%s", foods[i].name);
        scanf("%d", &foods[i].count);
        for (j = 0; j < foods[i].count; j++){
            scanf("%s", foods[i].ingredients[j]);
        }
    }

    int q;
    scanf("%d", &q);

    int qi;
    for (qi = 0; qi < q; qi++){
        char food1[MAX_NAME], food2[MAX_NAME];
        scanf("%s %s", food1, food2);

        int idx_food1 = -1, idx_food2 = -1;
        for (i = 0; i < n; i++){
            if (strcmp(foods[i].name, food1) == 0){
                idx_food1 = i;
            }
            if (strcmp(foods[i].name, food2) == 0){
                idx_food2 = i;
            }
        }

        char *common[20];
        int common_count = 0;
        for (i = 0; i < foods[idx_food1].count; i++){
            for (j = 0; j < foods[idx_food2].count; j++){
                if (strcmp(foods[idx_food1].ingredients[i], foods[idx_food2].ingredients[j]) == 0){
                    common[common_count] = foods[idx_food1].ingredients[i];
                    common_count++;
                }
            }
        }

        if (common_count == 0){
            printf("nothing");
        } else {
            qsort(common, common_count, sizeof(char *), comparestr);
            for (i = 0; i < common_count; i++){
                if (i > 0){
                    printf(" ");
                }
                printf("%s", common[i]);
            }
        }
        printf("\n");
    }


    return 0;
}