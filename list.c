#include <stdlib.h>
#include "list.h"

typedef struct ListNode {
    void* data;
    struct ListNode* next;
} ListNode;

struct List {
    ListNode* head;
    ListNode* tail;
    ListNode* current;
    int size;
};

List* list_create() {
    List* list = malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->size = 0;
    return list;
}

void list_pushBack(List* list, void* data) {
    ListNode* newNode = malloc(sizeof(ListNode));
    newNode->data = data;
    newNode->next = NULL;

    if (list->tail) {
        list->tail->next = newNode;
    } else {
        list->head = newNode;
    }

    list->tail = newNode;
    list->size++;
}

void* list_first(List* list) {
    if (!list || !list->head) return NULL;
    list->current = list->head;
    return list->current->data;
}

void* list_next(List* list) {
    if (!list || !list->current || !list->current->next) return NULL;
    list->current = list->current->next;
    return list->current->data;
}

void list_clean(List* list) {
    if (!list) return;
    ListNode* node = list->head;
    while (node) {
        ListNode* temp = node;
        node = node->next;
        free(temp->data);  // Asumiendo que el contenido fue reservado dinámicamente
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->size = 0;
}

void list_destroy(List* list) {
    if (!list) return;
    list_clean(list);
    free(list);
}

int list_size(List* list) {
    return list ? list->size : 0;
}

void* list_current(List* list) {
    if (!list || !list->current) return NULL;
    return list->current->data;
}

void list_popCurrent(List* list) {
    if (!list || !list->current) return;

    ListNode* prev = NULL;
    ListNode* node = list->head;

    while (node && node != list->current) {
        prev = node;
        node = node->next;
    }

    if (!node) return;

    if (node == list->head) {
        list->head = node->next;
    } else if (prev) {
        prev->next = node->next;
    }

    if (node == list->tail) {
        list->tail = prev;
    }

    list->current = node->next;
    free(node->data);
    free(node);
    list->size--;
}

