#ifndef LIST_H
#define LIST_H

typedef struct List List;

void* list_first(List* list);
void* list_next(List* list);
void* list_current(List* list);
void list_pushBack(List* list, void* data);
void list_popCurrent(List* list);
int list_size(List* list);
List* list_create();
void list_clean(List* list);
void list_destroy(List* list);

#endif
