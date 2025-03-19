#ifndef _LIST_H
#define _LIST_H

#define BOOL int
#define FALSE 0
#define TRUE 1

/* Definizione del record libro.
   - title: stringa (max 20 char + terminatore)
   - copies: numero di copie disponibili */
typedef struct {
    char title[21];
    int copies;
} ItemType;

/* Nodo della lista */
struct LINKED_LIST_NODE {
    ItemType item;
    struct LINKED_LIST_NODE* next;
};

typedef struct LINKED_LIST_NODE NODE;
typedef NODE* LIST;

/* Prototipi delle funzioni per la gestione della lista */
LIST NewList();
LIST DeleteList(LIST l);
BOOL isEmpty(LIST l);
int getLength(LIST l);
ItemType getHead(LIST l);
ItemType getTail(LIST l);
ItemType* Find(LIST l, ItemType item);
LIST EnqueueFirst(LIST l, ItemType item);
LIST EnqueueLast(LIST l, ItemType item);
LIST EnqueueOrdered(LIST l, ItemType item);
LIST DequeueFirst(LIST l);
LIST DequeueLast(LIST l);
LIST Dequeue(LIST l, ItemType item);
void PrintItem(ItemType item);
void PrintList(LIST l);

#endif
