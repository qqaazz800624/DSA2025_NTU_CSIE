#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_LEN 4000
#define MAX_TOKENS 4000  

typedef long long ll;

int precedence(char op) {
    if (op == '+' || op == '-'){
        return 1;
    }
    if (op == '*' || op == '/' || op == '%'){
        return 2;
    }
    return 0;
}

ll calculate(ll a, char op, ll b) {
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '%': return a % b;
    }
    return 0;
}

int infixToPostfix(char* infix, char **tokens){
    char opStack[MAX_LEN];
    int top = -1;
    int tokenCount = 0;
    int i = 0;
    int len = strlen(infix);

    while (i < len){
        if (isdigit(infix[i])){
            int j = 0;
            char numToken[MAX_LEN] = {0};
            while (i < len && isdigit(infix[i])){
                numToken[j++] = infix[i++];
            }
            numToken[j] = '\0';
            tokens[tokenCount] = malloc(strlen(numToken) + 1);
            strcpy(tokens[tokenCount], numToken);
            tokenCount++;
        } else {
            char c = infix[i];
            if (c == '('){
                opStack[++top] = c;
            } else if (c == ')'){
                while(top != -1 && opStack[top] != '('){
                    char op[2];
                    op[0] = opStack[top--];
                    op[1] = '\0';
                    tokens[tokenCount] = malloc(2);
                    strcpy(tokens[tokenCount], op);
                    tokenCount++;
                }
                if (top != -1 && opStack[top] == '('){
                    top--;
                }
            } else {
                while (top != -1 && opStack[top] != '(' && precedence(opStack[top]) >= precedence(c)){
                    char op[2];
                    op[0] = opStack[top--];
                    op[1] = '\0';
                    tokens[tokenCount] = malloc(2);
                    strcpy(tokens[tokenCount], op);
                    tokenCount++;
                }
                opStack[++top] = c;
            }
            i++;
        }
    }

    while(top != -1){
        char op[2];
        op[0] = opStack[top--];
        op[1] = '\0';
        tokens[tokenCount] = malloc(2);
        strcpy(tokens[tokenCount], op);
        tokenCount++;
    }

    return tokenCount;
}

ll evaluatePostfix(char **tokens, int tokenCount){
    ll stack[MAX_TOKENS];
    int top = -1;
    for (int i = 0; i < tokenCount; i++){
        if (isdigit(tokens[i][0])){
            ll num = atoll(tokens[i]);
            stack[++top] = num;
        } else {
            ll b = stack[top--];
            ll a = stack[top--];
            ll result = calculate(a, tokens[i][0], b);
            stack[++top] = result;
        }
    }
    return stack[top];
} 

int main(){
    char infix[MAX_LEN];

    char **tokens = malloc(MAX_TOKENS * sizeof(char*));
    if (tokens == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < 3; i++) {
        if (fgets(infix, sizeof(infix), stdin) == NULL) {
            break;  
        }
        int len = strlen(infix);
        if (len > 0 && infix[len - 1] == '\n') {
            infix[len - 1] = '\0';
        }
        
        int tokenCount = infixToPostfix(infix, tokens);
        
        char postfixStr[MAX_LEN] = "";
        for (int j = 0; j < tokenCount; j++) {
            strcat(postfixStr, tokens[j]);
        }
        
        ll result = evaluatePostfix(tokens, tokenCount);
        printf("%s=%lld\n", postfixStr, result);
        
        for (int j = 0; j < tokenCount; j++) {
            free(tokens[j]);
        }
    }
    
    free(tokens);
    return 0;
}