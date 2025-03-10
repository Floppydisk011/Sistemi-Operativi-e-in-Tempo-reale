#include <stdio.h>
#include "list.h"

int main(int argc, char* argv[]) {
    LIST mylist;
    ItemType i;

    mylist = NewList();
    printf("Fase di inizializzazione: Creata lista\n");

    printf("Fase di Controllo: ");
    if (isEmpty(mylist))
        printf(" La lista e' vuota\n");
    else
        printf(" La lista non e' vuota \n");

    printf("Fase di Inserimento dati\n");

    printf("La lista contiene %d elementi\n", getLength(mylist));

    printf("Contenuto della lista: [");
    PrintList(mylist);
    printf("]\n");

    i.value = 4.004;
    mylist = EnqueueLast(mylist, i);

    printf("La lista contiene %d elementi\n", getLength(mylist));

    i.value = 3.34;
    mylist = EnqueueLast(mylist, i);

    printf("La lista contiene %d elementi \n", getLength(mylist));

    i.value = 0.42;
    mylist = EnqueueLast(mylist, i);

    printf("La lista contiene %d elementi\n", getLength(mylist));

    i.value = 1.2345;
    mylist = EnqueueLast(mylist, i);

    printf("La lista contiene %d elementi\n", getLength(mylist));

    i.value = 1.1823;
    mylist = EnqueueLast(mylist, i);

    printf("La lista contiene %d elementi\n", getLength(mylist));

    i.value = 2.01;
    mylist = EnqueueFirst(mylist, i);

    printf("La lista contiene %d elementi\n", getLength(mylist));

    i.value = 3.11;
    mylist = EnqueueOrdered(mylist, i);

    printf("La lista contiene %d elementi\n", getLength(mylist));

    printf("Contenuto della lista: [");
    PrintList(mylist);
    printf("]\n");

    printf("Elemento di testa ");
    PrintItem(getHead(mylist));
    printf("\n");

    printf("Elemento di coda ");
    PrintItem(getTail(mylist));
    printf("\n");

    printf("Fase di test Find\n");
    i.value = 3.11;
    ItemType* found = Find(mylist, i);
    if (found){
        printf("Elemento %f trovato\n", found->value);
    }
    else{
        printf("Elemento %f non trovato\n", i.value);
    }

    i.value = 18;
    found = Find(mylist, i);
    if (found){
        printf("Elemento %f trovato\n", found->value);
    }
    else{
        printf("Elemento %f non trovato\n", i.value);
    }   

    printf("Fase di Estrazione dati\n");

    i.value = 1.2345;
    printf("Estrazione elemento ");
    PrintItem(i);
    printf("\n");
    mylist = Dequeue(mylist, i);

    i.value = 9.9999;
    printf("Estrazione elemento ");
    PrintItem(i);
    printf("\n");
    mylist = Dequeue(mylist, i);

    i.value = 4.004;
    printf("Estrazione elemento ");
    PrintItem(i);
    printf("\n");
    mylist = Dequeue(mylist, i);

    printf("Estrazione primo elemento\n");
    mylist = DequeueFirst(mylist);

    printf("Contenuto della lista: [");
    PrintList(mylist);
    printf("]\n");

    printf("Estrazione ultimo elemento\n");
    mylist = DequeueLast(mylist);

    printf("Contenuto della lista: [");
    PrintList(mylist);
    printf("]\n");

    printf("Fase di Distruzione dati\n");
    mylist = DeleteList(mylist);

    printf("Fase di Controllo: ");
    if (isEmpty(mylist))
        printf(" La lista e' vuota\n");
    else
        printf(" La lista non e' vuota \n");

    return 0;

} /* Main */
