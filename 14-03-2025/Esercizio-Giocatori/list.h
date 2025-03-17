#ifndef _LIST_H
#define _LIST_H

#define BOOL int
#define FALSE 0
#define TRUE (!FALSE)

/* Ridefiniamo ItemType per rappresentare il profilo del giocatore */
typedef struct {
    char name[50];
    int matches;  // numero di partite giocate
    int score;    // punteggio accumulato
} ItemType;

/* Nodo della lista */
struct LINKED_LIST_NODE {
    ItemType item;
    struct LINKED_LIST_NODE* next;
};

typedef struct LINKED_LIST_NODE NODE;
typedef NODE* LIST;

/* Costruttore/Distruttore */
LIST NewList();
LIST DeleteList(LIST l);

/* Predicati */
BOOL isEmpty(LIST l);
int getLength(LIST l);

/* Selettori */
ItemType getHead(LIST l);
ItemType getTail(LIST l);
ItemType* Find(LIST l, ItemType item);

/* Trasformatori */
LIST EnqueueFirst(LIST l, ItemType item);
LIST EnqueueLast(LIST l, ItemType item);
LIST EnqueueOrdered(LIST l, ItemType item);
LIST DequeueFirst(LIST l);
LIST DequeueLast(LIST l);
LIST Dequeue(LIST l, ItemType item);

/* Stampe */
void PrintItem(ItemType item);
void PrintList(LIST l);

#endif
