#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
#include <unistd.h>
#include <netdb.h>


#define BUF_SIZE 1000


int port = 8000;

LIST supplierList = NULL;
LIST centerList = NULL;

int greedy_match_supplier(ItemType supplier, LIST *centerList, LIST *chosenList) {
    int remaining = supplier.quantity;
    int total = 0;
    *chosenList = NewList();
    int found = 1;
    while (found) {
        found = 0;
        NODE *curr = *centerList, *prev = NULL;
        NODE *best = NULL, *bestPrev = NULL;
        while (curr != NULL) {
            if (curr->item.quantity <= remaining) {
                if (best == NULL || curr->item.quantity > best->item.quantity) {
                    best = curr;
                    bestPrev = prev;
                }
            }
            prev = curr;
            curr = curr->next;
        }
        if (best != NULL) {
            if (bestPrev == NULL)
                *centerList = best->next;
            else
                bestPrev->next = best->next;
            *chosenList = EnqueueLast(*chosenList, best->item);
            total += best->item.quantity;
            remaining -= best->item.quantity;
            free(best);
            found = 1;
        }
    }
    return total;
}

void process_supplier(NODE **supplierPrev, NODE **supplierCurr) {
    ItemType supplier = (*supplierCurr)->item;
    LIST chosenList;
    int total = greedy_match_supplier(supplier, &centerList, &chosenList);
    if (total >= supplier.min_req && !isEmpty(chosenList)) {
        char msg[BUF_SIZE] = "Centri assegnati: ";
        NODE *temp = chosenList;
        while (temp != NULL) {
            strcat(msg, temp->item.name);
            if (temp->next != NULL)
                strcat(msg, ", ");
            temp = temp->next;
        }
        send(supplier.sockfd, msg, strlen(msg)+1, 0);
        
        temp = chosenList;
        char msg_center[BUF_SIZE];
        sprintf(msg_center, "Fornitore assegnato: %s", supplier.name);
        while (temp != NULL) {
            send(temp->item.sockfd, msg_center, strlen(msg_center)+1, 0);
            close(temp->item.sockfd);
            temp = temp->next;
        }
        chosenList = DeleteList(chosenList);
        
        close(supplier.sockfd);
        
        NODE *toDelete = *supplierCurr;
        if (*supplierPrev == NULL)
            supplierList = toDelete->next;
        else
            (*supplierPrev)->next = toDelete->next;
        free(toDelete);
        
        if (*supplierPrev == NULL)
            *supplierCurr = supplierList;
        else
            *supplierCurr = (*supplierPrev)->next;
    }
}


int main() 
{
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    // Apertura socket
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);  
    if (sockfd == -1) {
        perror("Error opening socket");
        exit(1);
    }
    
    int options = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options)) < 0) {
        perror("Error on setsockopt");
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // Bind
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Error on binding");
        exit(1);
    }
    
    // Listen
    if (listen(sockfd, 20) == -1) {
        perror("Error on listen");
        exit(1);
    }

    socklen_t address_size = sizeof(cli_addr);    
    char buf[BUF_SIZE];    

    while (1) {
        printf("\nWaiting for a new connection...\n");
        
        // Accetta nuova connessione
        int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &address_size);      
        if (newsockfd == -1) {
            perror("Error on accept");
            exit(1);
        }
        
        bzero(buf, BUF_SIZE);
        
        // Ricezione del messaggio
        if (recv(newsockfd, buf, BUF_SIZE, 0) == -1) {
            perror("Error on receive");
            exit(1);
        }

        printf("Received from client: \"%s\"\n", buf);

        if (buf[0] == 'F') {
            // Formato: F nome quantity min_req
            ItemType s;
            s.sockfd = newsockfd;  // usa il file descriptor ottenuto da accept
            s.isSupplier = 1;
            if (sscanf(buf, "F %s %d %d", s.name, &s.quantity, &s.min_req) < 3) {
                char *err = "Formato fornitore errato";
                send(newsockfd, err, strlen(err)+1, 0);
                close(newsockfd);
                continue;
            }
            supplierList = EnqueueLast(supplierList, s);
            printf("Aggiunto fornitore: %s, qty: %d, min: %d\n", s.name, s.quantity, s.min_req);
        } else if (buf[0] == 'C') {
            // Formato: C nome demand
            ItemType c;
            c.sockfd = newsockfd;
            c.isSupplier = 0;
            c.min_req = 0; // non usato per i centri
            if (sscanf(buf, "C %s %d", c.name, &c.quantity) < 2) {
                char *err = "Formato centro errato";
                send(newsockfd, err, strlen(err)+1, 0);
                close(newsockfd);
                continue;
            }
            centerList = EnqueueLast(centerList, c);
            printf("Aggiunto centro: %s, domanda: %d\n", c.name, c.quantity);
        }
        
        // Processa i fornitori presenti nella lista
        NODE *supPrev = NULL;
        NODE *supCurr = supplierList;
        while (supCurr != NULL) {
            process_supplier(&supPrev, &supCurr);
            if (supCurr != NULL) {
                supPrev = supCurr;
                supCurr = supCurr->next;
            }
        }
    }

    close(sockfd);
    return 0;
}