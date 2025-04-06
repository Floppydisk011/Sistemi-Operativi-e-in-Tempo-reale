#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include "list.h"

#define BUF_SIZE 1000
#define PORT 8000
#define MAX_PENDING 100

/* Struttura per gestire le richieste pendenti dei lettori */
typedef struct {
    int sockfd;
    char title[21];
} PendingRequest;

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    
    /* Lista dei libri disponibili */
    LIST books = NewList();
    
    /* Array per richieste pendenti (client L) */
    PendingRequest pending[MAX_PENDING];
    int pendingCount = 0;
    
    /* Apertura del socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Errore apertura socket");
        exit(1);
    }
    
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Errore setsockopt");
        exit(1);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Errore bind");
        exit(1);
    }
    
    if (listen(sockfd, 20) < 0) {
        perror("Errore listen");
        exit(1);
    }
    
    clilen = sizeof(cli_addr);
    printf("Server in ascolto sulla porta %d...\n", PORT);
    
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("Errore accept");
            continue;
        }
        
        char buf[BUF_SIZE];
        memset(buf, 0, BUF_SIZE);
        if (recv(newsockfd, buf, BUF_SIZE, 0) < 0) {
            perror("Errore recv");
            close(newsockfd);
            continue;
        }
        
        /* Il protocollo prevede:
           - "C:<titolo>:<copie>" per i publisher (client C)
           - "L:<titolo>" per i lettori (client L) */
        printf("Messaggio ricevuto: \"%s\"\n", buf);
        
        if (buf[0] == 'C') {
            /* Gestione fornitura da parte di un publisher */
            char title[21];
            int copies;
            if (sscanf(buf, "C:%20[^:]:%d", title, &copies) != 2) {
                printf("Formato messaggio publisher non valido.\n");
                close(newsockfd);
                continue;
            }
            printf("Ricevuta fornitura: Libro \"%s\", copie %d\n", title, copies);
            
            /* Aggiunge il libro alla lista (si assume che non sia già presente) */
            ItemType newBook;
            strncpy(newBook.title, title, 21);
            newBook.copies = copies;
            books = EnqueueLast(books, newBook);
            
            printf("Lista aggiornata dei libri disponibili:\n");
            PrintList(books);
            
            /* Controlla se ci sono richieste pendenti per questo libro */
            for (int i = 0; i < pendingCount; ) {
                if (strcmp(pending[i].title, title) == 0 && newBook.copies > 0) {
                    char msg[BUF_SIZE];
                    snprintf(msg, BUF_SIZE, "Libro \"%s\" disponibile. Acquisto effettuato.", title);
                    if (send(pending[i].sockfd, msg, strlen(msg) + 1, 0) < 0)
                        perror("Errore invio a lettore pendente");
                    else
                        printf("Richiesta pendente soddisfatta per \"%s\".\n", title);
                    close(pending[i].sockfd);
                    /* Rimuove la richiesta pendente */
                    for (int j = i; j < pendingCount - 1; j++)
                        pending[j] = pending[j + 1];
                    pendingCount--;
                    newBook.copies--;  // una copia usata per soddisfare la richiesta
                } else {
                    i++;
                }
            }
            
            /* Aggiorna il record del libro nella lista */
            ItemType searchBook;
            strncpy(searchBook.title, title, 21);
            searchBook.copies = 0; // dummy
            ItemType* pBook = Find(books, searchBook);
            if (pBook != NULL) {
                pBook->copies = newBook.copies;
                if (pBook->copies <= 0) {
                    books = Dequeue(books, searchBook);
                    printf("Libro \"%s\" esaurito e rimosso dalla lista.\n", title);
                }
            }
            
            /* Chiusura della connessione col publisher */
            close(newsockfd);
        } else if (buf[0] == 'L') {
            /* Gestione richiesta da parte di un lettore */
            char title[21];
            if (sscanf(buf, "L:%20s", title) != 1) {
                printf("Formato messaggio lettore non valido.\n");
                close(newsockfd);
                continue;
            }
            printf("Richiesta di acquisto per il libro \"%s\".\n", title);
            
            ItemType searchBook;
            strncpy(searchBook.title, title, 21);
            searchBook.copies = 0;
            ItemType* pBook = Find(books, searchBook);
            if (pBook != NULL && pBook->copies > 0) {
                /* Libro disponibile: invia risposta positiva */
                char msg[BUF_SIZE];
                snprintf(msg, BUF_SIZE, "Libro \"%s\" disponibile. Acquisto effettuato.", title);
                if (send(newsockfd, msg, strlen(msg) + 1, 0) < 0)
                    perror("Errore invio a lettore");
                close(newsockfd);
                pBook->copies--;
                if (pBook->copies <= 0) {
                    books = Dequeue(books, searchBook);
                    printf("Libro \"%s\" esaurito e rimosso dalla lista.\n", title);
                }
            } else {
                /* Libro non disponibile: aggiunge la richiesta in coda */
                if (pendingCount < MAX_PENDING) {
                    strncpy(pending[pendingCount].title, title, 21);
                    pending[pendingCount].sockfd = newsockfd;
                    pendingCount++;
                    printf("Richiesta per \"%s\" messa in attesa.\n", title);
                    /* La connessione resta aperta in attesa di una fornitura futura */
                } else {
                    printf("Numero massimo di richieste pendenti raggiunto.\n");
                    close(newsockfd);
                }
            }
        } else {
            printf("Tipo di messaggio sconosciuto.\n");
            close(newsockfd);
        }
    }
    
    close(sockfd);
    return 0;
}
