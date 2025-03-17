#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

/* Confronta due profili: usiamo il nome per l’ordinamento */
int itemCompare(ItemType item1, ItemType item2) {
    return strcmp(item1.name, item2.name);
}

/* Alloca un nuovo nodo per la lista */
NODE* createNode(ItemType item) {
    NODE* p = (NODE*)malloc(sizeof(NODE));
    assert(p != NULL);
    p->item = item;
    p->next = NULL;
    return p;
}

/* Dealloca il nodo p */
void deleteNode(NODE* p) {
    free(p);
}

/* Inizializza una nuova lista */
LIST NewList() {
    return NULL;
}

/* Azzera la lista */
LIST DeleteList(LIST l) {
    LIST tmp = l;
    while (!isEmpty(tmp)) {
        NODE* todel = tmp;
        tmp = tmp->next;
        deleteNode(todel);
    }
    return NewList();
}

/* Ritorna TRUE se la lista è vuota */
BOOL isEmpty(LIST l) {
    return l == NULL;
}

/* Ritorna il numero di elementi presenti nella lista */
int getLength(LIST l) {
    int size = 0;
    LIST tmp = l;
    while (!isEmpty(tmp)) {
        ++size;
        tmp = tmp->next;
    }
    return size;
}

/* Restituisce l'elemento in testa alla lista */
ItemType getHead(LIST l) {
    assert(!isEmpty(l));
    return l->item;
}

/* Restituisce l'elemento in coda alla lista */
ItemType getTail(LIST l) {
    NODE* tmp = l;
    assert(!isEmpty(l));
    while (!isEmpty(tmp->next))
        tmp = tmp->next;
    return tmp->item;
}

/* Cerca il profilo nella lista e restituisce il puntatore; NULL se non trovato */
ItemType* Find(LIST l, ItemType item) {
    LIST tmp = l;
    while (tmp != NULL && itemCompare(tmp->item, item) != 0)
        tmp = tmp->next;
    if (tmp == NULL)
        return NULL;
    return &(tmp->item);
}

/* Inserisce un elemento nella prima posizione della lista */
LIST EnqueueFirst(LIST l, ItemType item) {
    NODE* new_node = createNode(item);
    new_node->next = l;
    return new_node;
}

/* Inserisce un elemento nell'ultima posizione della lista */
LIST EnqueueLast(LIST l, ItemType item) {
    NODE* new_node = createNode(item);
    if (isEmpty(l)) {
        l = new_node;
    } else {
        LIST tmp = l;
        while (!isEmpty(tmp->next))
            tmp = tmp->next;
        tmp->next = new_node;
    }
    return l;
}

/* Inserisce un elemento mantenendo la lista ordinata (crescente per nome) */
LIST EnqueueOrdered(LIST l, ItemType item) {
    NODE* new_node = createNode(item);
    if (l == NULL || itemCompare(item, l->item) < 0) {
        new_node->next = l;
        return new_node;
    }
    NODE* curr = l;
    while (curr->next != NULL && itemCompare(curr->next->item, item) < 0)
        curr = curr->next;
    new_node->next = curr->next;
    curr->next = new_node;
    return l;
}

/* Rimuove il primo elemento della lista */
LIST DequeueFirst(LIST l) {
    if (!isEmpty(l)) {
        NODE* todel = l;
        l = l->next;
        deleteNode(todel);
    }
    return l;
}

/* Rimuove l'ultimo elemento della lista */
LIST DequeueLast(LIST l) {
    if (l == NULL)
        return l;
    if (l->next == NULL) {
        deleteNode(l);
        return NULL;
    }
    NODE* curr = l;
    while (curr->next->next != NULL)
        curr = curr->next;
    deleteNode(curr->next);
    curr->next = NULL;
    return l;
}

/* Rimuove un elemento specifico dalla lista */
LIST Dequeue(LIST l, ItemType item) {
    if (!isEmpty(l)) {
        if (itemCompare(l->item, item) == 0) {
            NODE* todel = l;
            l = l->next;
            deleteNode(todel);
        } else {
            LIST tmp = l;
            while (!isEmpty(tmp->next) && itemCompare(tmp->next->item, item) != 0)
                tmp = tmp->next;
            if (!isEmpty(tmp->next)) {
                NODE* todel = tmp->next;
                tmp->next = tmp->next->next;
                deleteNode(todel);
            }
        }
    }
    return l;
}

/* Stampa a video un profilo giocatore */
void PrintItem(ItemType item) {
    printf("Nome: %s, Partite: %d, Punteggio: %d\n", item.name, item.matches, item.score);
}

/* Stampa a video tutti i profili nella lista */
void PrintList(LIST l) {
    LIST tmp = l;
    while (!isEmpty(tmp)) {
        PrintItem(tmp->item);
        tmp = tmp->next;
    }
}
