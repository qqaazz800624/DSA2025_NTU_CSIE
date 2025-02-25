#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE (2 << 20)
char string[BUFFER_SIZE] = "";

int main(){

    char command[20];
    char argument[2][20];
    int i;
    while (scanf("%s", command) != EOF){
        int pos;
        if (strcmp(command, "insert") == 0){
            scanf("%s %s", argument[0], argument[1]);
            if (strcmp(argument[0], "left") == 0){
                pos = 0;
            } else if (strcmp(argument[0], "right") == 0){
                pos = strlen(string);
            } else {
                sscanf(argument[0], "%d", &pos);
                pos = pos - 1;
            }
            int current_length = strlen(string);
            string[current_length + 1] = '\0';
            for (i = current_length - 1; i >= pos; i--){
                string[i + 1] = string[i];
            }
            string[pos] = argument[1][0];
        } else if (strcmp(command, "delete") == 0){
            scanf("%s", argument[0]);
            if (strcmp(argument[0], "left") == 0){
                pos = 0;            
            } else if (strcmp(argument[0], "right") == 0){
                pos = strlen(string) - 1;
            } else {
                sscanf(argument[0], "%d", &pos);
                pos = pos - 1;
            }

            int current_length = strlen(string);
            for (i = pos; i < current_length; i++){
                string[i] = string[i + 1];
            }
        }
    }

    int longest = 0;
    int run = 1;
    int len = strlen(string);

    for (i = 0; i < len; i++){
        if (i < len - 1 && string[i] == string[i + 1]){
            run++;
        } else {
            if (run > longest){
                longest = run;
            }
            run = 1;
        }
    }

    run = 1;
    for (i = 0; i < len; i++){
        if (i < len - 1 && string[i] == string[i + 1]){
            run++;
        } else {
            if (run == longest){
                printf("%c ", string[i]);
            }
            run = 1;
        }
    }
    printf("%d\n", longest);

    return 0;
}