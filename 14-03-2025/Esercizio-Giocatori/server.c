#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>
#include "list.h"

#define BUF_SIZE 1000
#define PORT 8000
#define N_PLAYERS 3  /* Numero minimo di giocatori per simulare la partita */

typedef struct {
    int sockfd;
    char name[50];
} WaitingPlayer;

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    /* Lista globale dei giocatori registrati */
    LIST allPlayers = NewList();

    /* Array per memorizzare i giocatori in attesa per la partita corrente */
    WaitingPlayer waitingPlayers[N_PLAYERS];
    int waitingCount = 0;

    /* Inizializza il seme per la randomizzazione */
    srand(time(NULL));

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

    while (1) {
        printf("\nIn attesa di nuove connessioni...\n");
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("Errore accept");
            exit(1);
        }

        /* Ricezione del nome del giocatore dal client */
        char buf[BUF_SIZE];
        memset(buf, 0, BUF_SIZE);
        if (recv(newsockfd, buf, BUF_SIZE, 0) < 0) {
            perror("Errore recv");
            close(newsockfd);
            continue;
        }
        buf[strcspn(buf, "\n")] = '\0';  // rimuove eventuale newline
        printf("Giocatore connesso: %s\n", buf);

        /* Controlla se il profilo esiste già, altrimenti lo crea */
        ItemType searchItem;
        strncpy(searchItem.name, buf, sizeof(searchItem.name));
        searchItem.matches = 0;
        searchItem.score = 0;

        ItemType* pProfile = Find(allPlayers, searchItem);
        if (pProfile == NULL) {
            ItemType newProfile;
            strncpy(newProfile.name, buf, sizeof(newProfile.name));
            newProfile.matches = 0;
            newProfile.score = 0;
            allPlayers = EnqueueLast(allPlayers, newProfile);
            printf("Nuovo profilo creato per %s\n", newProfile.name);
        } else {
            printf("Profilo esistente per %s\n", buf);
        }

        /* Aggiunge il giocatore alla lista di attesa per la partita */
        strncpy(waitingPlayers[waitingCount].name, buf, sizeof(waitingPlayers[waitingCount].name));
        waitingPlayers[waitingCount].sockfd = newsockfd;
        waitingCount++;

        /* Stampa l'elenco dei giocatori registrati */
        printf("\nLista giocatori registrati:\n");
        PrintList(allPlayers);

        /* Stampa i giocatori in attesa per la partita */
        printf("\nGiocatori in attesa per la partita:\n");
        for (int i = 0; i < waitingCount; i++) {
            printf("%s\n", waitingPlayers[i].name);
        }

        /* Se il numero dei giocatori raggiunge N_PLAYERS, simula la partita */
        if (waitingCount == N_PLAYERS) {
            /* Mescola casualmente l’array dei giocatori in attesa */
            for (int i = waitingCount - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                WaitingPlayer temp = waitingPlayers[i];
                waitingPlayers[i] = waitingPlayers[j];
                waitingPlayers[j] = temp;
            }
            /* Aggiorna i profili: incrementa il numero di partite e assegna i punti ai primi tre */
            for (int i = 0; i < waitingCount; i++) {
                ItemType searchItem;
                strncpy(searchItem.name, waitingPlayers[i].name, sizeof(searchItem.name));
                searchItem.matches = 0;
                searchItem.score = 0;
                ItemType* profile = Find(allPlayers, searchItem);
                if (profile != NULL) {
                    profile->matches += 1;
                    if (i == 0)
                        profile->score += 3;
                    else if (i == 1)
                        profile->score += 2;
                    else if (i == 2)
                        profile->score += 1;
                }
            }

            /* Costruisce il messaggio di risultato */
            char resultMsg[BUF_SIZE];
            snprintf(resultMsg, BUF_SIZE,
                     "Risultati partita:\n1°: %s (3 punti)\n2°: %s (2 punti)\n3°: %s (1 punto)\n",
                     waitingPlayers[0].name, waitingPlayers[1].name, waitingPlayers[2].name);

            /* Invia il risultato a tutti i client in attesa e chiude la connessione */
            for (int i = 0; i < waitingCount; i++) {
                if (send(waitingPlayers[i].sockfd, resultMsg, strlen(resultMsg) + 1, 0) < 0)
                    perror("Errore invio risultato");
                close(waitingPlayers[i].sockfd);
            }
            waitingCount = 0;  // azzera la lista di attesa

            /* Stampa l'aggiornamento dei profili */
            printf("\nAggiornamento profili giocatori:\n");
            PrintList(allPlayers);
        }
    }

    close(sockfd);
    return 0;
}
