#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>


#include "list.h"

#define BUF_SIZE 1000
#define PORT 8000

/* Il server utilizza la lista per gestire le guide in attesa */
LIST guide_list;

void start_tour(NODE* candidate) {
    char msg[BUF_SIZE];
    /* Invia al processo guida il numero totale di visitatori */
    sprintf(msg, "%d", candidate->item.current);
    if (send(candidate->item.sock, msg, strlen(msg)+1, 0) == -1) {
        perror("Errore nell'invio al processo guida");
    } else {
        printf("Avvio visita per la guida %s: inviato totale %s\n", candidate->item.name, msg);
    }
    close(candidate->item.sock);

    /* Invia a ciascun gruppo di visitatori il nome della guida */
    for (int i = 0; i < candidate->item.visitor_count; i++) {
        if (send(candidate->item.visitor_socks[i], candidate->item.name, strlen(candidate->item.name)+1, 0) == -1) {
            perror("Errore nell'invio al gruppo di visitatori");
        } else {
            printf("Inviato al gruppo di visitatori (socket %d): guida %s\n", candidate->item.visitor_socks[i], candidate->item.name);
        }
        close(candidate->item.visitor_socks[i]);
    }

    /* Rimuove la guida dalla lista: il confronto avviene sul nome,
       quindi possiamo usare Dequeue() passandole il candidato */
    guide_list = Dequeue(guide_list, candidate->item);
}

int main() {
    int newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    char buf[BUF_SIZE];

    guide_list = NewList();

    int sockfd = socket( PF_INET, SOCK_STREAM, 0 );  
	if ( sockfd == -1 ) 
	{
		perror("Error opening socket");
		exit(1);
	}

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Errore in setsockopt");
        exit(1);
    }

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Errore nel binding");
        exit(1);
    }

    listen(sockfd, 20);
    printf("Server in ascolto sulla porta %d\n", PORT);
	socklen_t address_size = sizeof( cli_addr );	

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &address_size);
        if (newsockfd < 0) {
            perror("Errore in accept");
            continue;
        }
        printf("Connessione accettata (socket %d)\n", newsockfd);

        memset(buf, 0, BUF_SIZE);
        if ( recv( newsockfd, buf, BUF_SIZE, 0 ) == -1) 
		{
			perror("Error on receive");
			exit(1);
		}
        printf("Messaggio ricevuto: %s\n", buf);


        if (buf[0] == 'G') {
            ItemType new_guide;
            new_guide.sock = newsockfd;  
            new_guide.current = 0;
            new_guide.visitor_count = 0;
            memset(new_guide.visitor_socks, 0, sizeof(new_guide.visitor_socks));

            char *token = strtok(buf, ";");
            token = strtok(NULL, ";"); 
            if (token == NULL) { close(newsockfd); continue; }
            strncpy(new_guide.name, token, 30);
            new_guide.name[30] = '\0';

            token = strtok(NULL, ";"); 
            if (token == NULL) { close(newsockfd); continue; }
            new_guide.min = atoi(token);

            token = strtok(NULL, ";"); 
            if (token == NULL) { close(newsockfd); continue; }
            new_guide.max = atoi(token);

            guide_list = EnqueueLast(guide_list, new_guide);
            printf("Registrata guida: %s (min: %d, max: %d)\n", new_guide.name, new_guide.min, new_guide.max);

        }
        else if (buf[0] == 'V') {
            int group_size = 0;
            char *token = strtok(buf, ";");
            token = strtok(NULL, ";"); 
            if (token == NULL) { close(newsockfd); continue; }
            group_size = atoi(token);

            NODE* candidate = NULL;
            int best_slack = 0;
            for (NODE* tmp = guide_list; tmp != NULL; tmp = tmp->next) {
                int available = tmp->item.max - tmp->item.current;
                if (available >= group_size) {
                    int slack = available - group_size;
                    if (candidate == NULL || slack < best_slack) {
                        candidate = tmp;
                        best_slack = slack;
                    }
                }
            }
            if (candidate == NULL) {
                char msg[] = "NO_GUIDE";
                send(newsockfd, msg, strlen(msg)+1, 0);
                printf("Nessuna guida disponibile per un gruppo di %d visitatori\n", group_size);
                close(newsockfd);
            } else {
                if (candidate->item.visitor_count < MAX_VISITOR_GROUPS) {
                    candidate->item.visitor_socks[candidate->item.visitor_count] = newsockfd;
                    candidate->item.visitor_count++;
                    candidate->item.current += group_size;
                    printf("Gruppo di %d visitatori assegnato alla guida %s; totale ora %d\n",
                           group_size, candidate->item.name, candidate->item.current);

                    if (candidate->item.current >= candidate->item.min) {
                        printf("Avvio della visita per la guida %s\n", candidate->item.name);
                        start_tour(candidate);
                    }
                } else {
                    char msg[] = "NO_GUIDE";
                    send(newsockfd, msg, strlen(msg)+1, 0);
                    close(newsockfd);
                }
            }
        }
        else {
            printf("Tipo di messaggio sconosciuto\n");
            close(newsockfd);
        }
    }

    close(sockfd);  
    return 0;
}
