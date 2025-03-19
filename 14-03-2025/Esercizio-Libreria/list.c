#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "list.h"

/* Confronta due libri in base al titolo */
int itemCompare(ItemType item1, ItemType item2) {
    return strcmp(item1.title, item2.title);
}

/* Alloca un nuovo nodo per la lista */
NODE* createNode(ItemType item) {
    NODE* p = (NODE*)malloc(sizeof(NODE));
    assert(p != NULL);
    p->item = item;
    p->next = NULL;
    return p;
}

/* Dealloca un nodo */
void deleteNode(NODE* p) {
    free(p);
}

/* Inizializza una nuova lista vuota */
LIST NewList() {
    return NULL;
}

/* Dealloca tutti i nodi della lista */
LIST DeleteList(LIST l) {
    while (!isEmpty(l)) {
        NODE* temp = l;
        l = l->next;
        deleteNode(temp);
    }
    return NewList();
}

/* Ritorna TRUE se la lista è vuota */
BOOL isEmpty(LIST l) {
    return (l == NULL);
}

/* Ritorna il numero di elementi nella lista */
int getLength(LIST l) {
    int count = 0;
    while (l) {
        count++;
        l = l->next;
    }
    return count;
}

/* Restituisce il primo elemento della lista */
ItemType getHead(LIST l) {
    assert(!isEmpty(l));
    return l->item;
}

/* Restituisce l'ultimo elemento della lista */
ItemType getTail(LIST l) {
    assert(!isEmpty(l));
    NODE* tmp = l;
    while (tmp->next)
        tmp = tmp->next;
    return tmp->item;
}

/* Cerca un libro nella lista in base al titolo */
ItemType* Find(LIST l, ItemType item) {
    while (l) {
        if (itemCompare(l->item, item) == 0)
            return &(l->item);
        l = l->next;
    }
    return NULL;
}

/* Inserisce un elemento in testa */
LIST EnqueueFirst(LIST l, ItemType item) {
    NODE* new_node = createNode(item);
    new_node->next = l;
    return new_node;
}

/* Inserisce un elemento in coda */
LIST EnqueueLast(LIST l, ItemType item) {
    NODE* new_node = createNode(item);
    if (isEmpty(l))
        return new_node;
    NODE* tmp = l;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = new_node;
    return l;
}

/* Inserisce un elemento mantenendo l'ordinamento per titolo */
LIST EnqueueOrdered(LIST l, ItemType item) {
    NODE* new_node = createNode(item);
    if (l == NULL || itemCompare(item, l->item) < 0) {
        new_node->next = l;
        return new_node;
    }
    NODE* current = l;
    while (current->next && itemCompare(current->next->item, item) < 0)
        current = current->next;
    new_node->next = current->next;
    current->next = new_node;
    return l;
}

/* Rimuove il primo elemento della lista */
LIST DequeueFirst(LIST l) {
    if (!isEmpty(l)) {
        NODE* temp = l;
        l = l->next;
        deleteNode(temp);
    }
    return l;
}

/* Rimuove l'ultimo elemento della lista */
LIST DequeueLast(LIST l) {
    if (isEmpty(l))
        return l;
    if (l->next == NULL) {
        deleteNode(l);
        return NULL;
    }
    NODE* current = l;
    while (current->next->next)
        current = current->next;
    deleteNode(current->next);
    current->next = NULL;
    return l;
}

/* Rimuove un elemento specifico dalla lista (in base al titolo) */
LIST Dequeue(LIST l, ItemType item) {
    if (isEmpty(l))
        return l;
    if (itemCompare(l->item, item) == 0) {
        NODE* temp = l;
        l = l->next;
        deleteNode(temp);
        return l;
    }
    NODE* current = l;
    while (current->next && itemCompare(current->next->item, item) != 0)
        current = current->next;
    if (current->next) {
        NODE* temp = current->next;
        current->next = current->next->next;
        deleteNode(temp);
    }
    return l;
}

/* Stampa un record libro */
void PrintItem(ItemType item) {
    printf("Libro: %s, copie: %d\n", item.title, item.copies);
}

/* Stampa tutti i libri della lista */
void PrintList(LIST l) {
    while (l) {
        PrintItem(l->item);
        l = l->next;
    }
}
