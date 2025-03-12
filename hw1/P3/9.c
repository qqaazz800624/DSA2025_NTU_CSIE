#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "9.h"

struct Node* head;
struct Node* tail;
int next_node_id = 1;
int list_length = 0;

/*
Finish your implementation for the 3 routines and 7 types of operations HERE
*/


// Pseudo-code:
// 1. next = Next_Node(node, prev)
// 2. newNode = New_XOR_Node(next ⊕ node)
// 3. next->neighbors = next->neighbors ⊕ node ⊕ newNode    (if next != NULL)
// 4. node->neighbors = prev ⊕ newNode
// update list_length for insertion

void Insert_After(struct Node* node, struct Node* prev) {
    struct Node* next = Next_Node(node, prev);
    struct Node* newNode = New_XOR_Node((struct Node*)((uintptr_t)node ^ (uintptr_t)next));
    if (next != NULL) {
        next->neighbors = (struct Node*)((uintptr_t)next->neighbors ^ (uintptr_t)node ^ (uintptr_t)newNode);
    }
    node->neighbors = (struct Node*)((uintptr_t)prev ^ (uintptr_t)newNode);
    if (node == tail) {
        tail = newNode;
    }
    list_length++;
}

// Pseudo-code:
// 1. next = Next_Node(node, prev)
// 2. prev->neighbors = prev->neighbors ⊕ node ⊕ next   (if prev != NULL)
// 3. next->neighbors = next->neighbors ⊕ node ⊕ prev   (if next != NULL)
// 4. free(node) and decrement list_length

void Remove_Here(struct Node* node, struct Node* prev) {
    struct Node* next = Next_Node(node, prev);
    if (prev != NULL) {
        prev->neighbors = (struct Node*)((uintptr_t)prev->neighbors ^ (uintptr_t)node ^ (uintptr_t)next);
    } else {
        head = next;
    }
    if (next != NULL) {
        next->neighbors = (struct Node*)((uintptr_t)next->neighbors ^ (uintptr_t)node ^ (uintptr_t)prev);
    } else {
        tail = prev;
    }
    free(node);
    list_length--;
}

// Pseudo-code:
// 1. prev->neighbors = prev->neighbors ⊕ begin ⊕ end   (if prev exists)
// 2. begin->neighbors = begin->neighbors ⊕ prev ⊕ next
// 3. end->neighbors = end->neighbors ⊕ next ⊕ prev
// 4. next->neighbors = next->neighbors ⊕ end ⊕ begin   (if next exists)

void Reverse(struct Node* prev, struct Node* begin, struct Node* end, struct Node* next) {
    if (prev != NULL) {
        prev->neighbors = (struct Node*)((uintptr_t)prev->neighbors ^ (uintptr_t)begin ^ (uintptr_t)end);
    }
    if (begin != NULL) {
        begin->neighbors = (struct Node*)((uintptr_t)begin->neighbors ^ (uintptr_t)prev ^ (uintptr_t)next);
    }
    if (end != NULL) {
        end->neighbors = (struct Node*)((uintptr_t)end->neighbors ^ (uintptr_t)next ^ (uintptr_t)prev);
    }
    if (next != NULL) {
        next->neighbors = (struct Node*)((uintptr_t)next->neighbors ^ (uintptr_t)end ^ (uintptr_t)begin);
    }
}

// Helper function: Traverse to the k-th node (1-indexed).
// Returns pointer to the k-th node and sets *out_prev to its previous node.
static struct Node* traverse_to(int k, struct Node** out_prev) {
    if (k < 1 || k > list_length) return NULL;
    struct Node* curr = head;
    struct Node* prev = NULL;
    for (int i = 1; i < k && curr != NULL; i++){
         struct Node* next = Next_Node(curr, prev);
         prev = curr;
         curr = next;
    }
    if (out_prev)
        *out_prev = prev;
    return curr;
}


// Helper function: Get the length of the XOR linked list.
static int getLength(){
    return list_length;
}

// type_0(k): Return the data field of the k-th node (1-indexed).
int type_0(int k) {
    struct Node* prev = NULL;
    struct Node* curr = traverse_to(k, &prev);
    return curr ? curr->data : -1;
}

// type_1(): Insert-After at head (with prev = NULL). The new node becomes the new head.
void type_1() {
    if (head == NULL) {
        head = tail = New_XOR_Node(NULL);
        head->neighbors = NULL;
        list_length = 1;
    } else {
        // Special-case for head insertion.
        struct Node* newNode = New_XOR_Node(head); // newNode->neighbors = head (since XOR(NULL, head)=head)
        // Update the old head's neighbor field: originally head->neighbors = (NULL XOR next) = next.
        // Now update it to: (newNode XOR next)
        head->neighbors = (struct Node*)((uintptr_t)head->neighbors ^ (uintptr_t)newNode);
        head = newNode;
        list_length++;
    }
}

// type_2(k): Insert-After at the k-th node (1-indexed). New node becomes (k+1)-th node.
void type_2(int k) {
    struct Node* prev = NULL;
    struct Node* curr = traverse_to(k, &prev);
    if (curr != NULL)
        Insert_After(curr, prev);
}

// type_3(k): Insert-After at the k-th last node. New node becomes k-th last node.
void type_3(int k) {
    if (k < 1 || k > list_length) return;
    int position = list_length - k + 1;
    struct Node* prev = NULL;
    struct Node* curr = traverse_to(position, &prev);
    if (curr != NULL)
        Insert_After(curr, prev);
}

// type_4(k): Remove_Here at the k-th node (1-indexed).
void type_4(int k) {
    struct Node* prev = NULL;
    struct Node* curr = traverse_to(k, &prev);
    if (curr != NULL)
        Remove_Here(curr, prev);
}

// type_5(k): Remove_Here at the k-th last node.
void type_5(int k) {
    if (k < 1 || k > list_length) return;
    int position = list_length - k + 1;
    struct Node* prev = NULL;
    struct Node* curr = traverse_to(position, &prev);
    if (curr != NULL)
        Remove_Here(curr, prev);
}

// type_6(k): Reverse from the k-th node to the k-th last node.
void type_6(int k) {
    int len = getLength();
    int start = k;
    int end = len - k + 1;
    if (start < 1 || end > len || start >= end) {
        return;
    }
    struct Node* prev = NULL;
    struct Node* begin = traverse_to(start, &prev);
    struct Node* rprev = NULL;
    struct Node* endNode = traverse_to(end, &rprev);
    struct Node* next = Next_Node(endNode, rprev);
    if (begin != NULL && endNode != NULL) {
        Reverse(prev, begin, endNode, next);
    }
}