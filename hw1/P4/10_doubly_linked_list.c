#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    long long value;
    struct Node *prev, *next;
} Node;

typedef struct {
    Node *head, *tail;
    int size;
} Diamonds;

void init(Diamonds *d);
void process_type_1(Diamonds *d, int Ni, long long vi);
void process_type_2(Diamonds *d, long long pi);
void process_type_3(Diamonds *d, long long M);
void free_list(Diamonds *d);

int main() {
    int T;
    long long M;
    scanf("%d %lld", &T, &M);

    Diamonds d;
    init(&d);

    for (int i = 0; i < T; i++) {
        int op;
        scanf("%d", &op);
        if (op == 1) {
            int Ni;
            long long vi;
            scanf("%d %lld", &Ni, &vi);
            process_type_1(&d, Ni, vi);
        } else if (op == 2) {
            long long pi;
            scanf("%lld", &pi);
            process_type_2(&d, pi);
        } else {
            process_type_3(&d, M);
        }
    }
    free_list(&d);
    return 0;
}


void init(Diamonds *d) {
    d->head = d->tail = NULL;
    d->size = 0;
}

void process_type_1(Diamonds *d, int Ni, long long vi) {
    Node *cur = d->tail, *temp;
    int removed = 0;

    while (cur && cur->value < vi) {
        temp = cur;
        cur = cur->prev;
        if (cur) {
            cur->next = NULL;
        } else {
            d->head = NULL;
        }
        free(temp);
        d->size--;
        removed++;
    }
    d->tail = cur;
    printf("%d\n", removed);

    Node *insert_after = d->tail;
    while (insert_after && insert_after->value < vi){
        insert_after = insert_after->next;
    }

    for (int i = 0; i < Ni; i++) {
        Node *newNode = malloc(sizeof(Node));
        newNode->value = vi;
        newNode->prev = insert_after;
        if (insert_after){
            newNode->next = insert_after->next;
        } else {
            newNode->next = d->head;
        }
        if (insert_after) {
            insert_after->next = newNode;
        }
        if (newNode->next) {
            newNode->next->prev = newNode;
        }
        if (!newNode->prev) {
            d->head = newNode;
        }
        if (!newNode->next) {
            d->tail = newNode;
        }
        insert_after = newNode;
        d->size++;
    }
}

void process_type_2(Diamonds *d, long long pi) {
    int cnt = 0;
    Node *cur = d->head;
    while (cur && cur->value >= pi) {
        if (cur->value == pi) {
            cnt++;
        }
        cur = cur->next;
    }
    printf("%d\n", cnt);
}

void process_type_3(Diamonds *d, long long M) {
    Node *cur = d->head;
    int rank = 1;
    while (cur) {
        cur->value += (M - rank + 1);
        rank++;
        cur = cur->next;
    }
}

void free_list(Diamonds *d) {
    Node *cur = d->head, *temp;
    while (cur) {
        temp = cur;
        cur = cur->next;
        free(temp);
    }
}

