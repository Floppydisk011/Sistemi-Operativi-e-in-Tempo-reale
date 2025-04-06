#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>
#include "list.h"

#define BUF_SIZE 1000
int port = 8000;
LIST offers = NULL;

int main() 
{
    struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	// Socket opening
	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );  
	if ( sockfd == -1 ) 
	{
		perror("Error opening socket");
		exit(1);
	}
	
	int options = 1;
	if(setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof (options)) < 0) {
		perror("Error on setsockopt");
		exit(1);
	}

	bzero( &serv_addr, sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	// Address bindind to socket
	if ( bind( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) == -1 ) 
	{
		perror("Error on binding");
		exit(1);
	}
    
    if (listen(sockfd, 20) == -1) {
        perror("Error on listen");
        exit(1);
    }
    
    socklen_t address_size = sizeof(cli_addr);
    char buf[BUF_SIZE];
    
    printf("Server in esecuzione sulla porta %d...\n", port);
    
    while (1) {
        printf("\nIn attesa di una nuova connessione...\n");
        int newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
		if (newsockfd == -1) 
		{
			perror("Error on accept");
			exit(1);
		}
		
		bzero(buf, BUF_SIZE);
        
        int r = recv(newsockfd, buf, BUF_SIZE, 0);
        if (r < 0) {
            perror("Error on receive");
            close(newsockfd);
            continue;
        }
        printf("Messaggio ricevuto: \"%s\"\n", buf);
        
        if (buf[0] == 'A') {
            char agentName[31];
            int quantity, price, minPrice;
            sscanf(buf, "A %30s %d %d %d", agentName, &quantity, &price, &minPrice);
            ItemType offer;
            strncpy(offer.agentName, agentName, 31);
            offer.quantity = quantity;
            offer.price = price;
            offer.minPrice = minPrice;
            offer.revenue = 0;
            offer.sock = newsockfd;
            offers = EnqueueLast(offers, offer);
            printf("Agente registrato: %s, Quantità: %d, Prezzo: %d, PrezzoMin: %d\n",
                   agentName, quantity, price, minPrice);
            
        } else if (buf[0] == 'I') {
            char listBuffer[BUF_SIZE * 2];
            listBuffer[0] = '\0';
            NODE* tmp = offers;
            while (tmp != NULL) {
                char line[100];
                sprintf(line, "%s %d %d %d\n", tmp->item.agentName, tmp->item.quantity, tmp->item.price, tmp->item.minPrice);
                strcat(listBuffer, line);
                tmp = tmp->next;
            }
            if (strlen(listBuffer) == 0) {
                strcpy(listBuffer, "Nessuna offerta registrata\n");
                send(newsockfd, listBuffer, strlen(listBuffer) + 1, 0);
                close(newsockfd);
                continue;
            }
            if (send(newsockfd, listBuffer, strlen(listBuffer) + 1, 0) == -1) {
                perror("Error sending list to investor");
                close(newsockfd);
                continue;
            }
            printf("Lista inviata all'investitore:\n%s\n", listBuffer);
            
            char chosen[31];
            memset(buf, 0, BUF_SIZE);
            r = recv(newsockfd, chosen, 31, 0);
            if (r <= 0) {
                perror("Error receiving chosen agent");
                close(newsockfd);
                continue;
            }
            printf("Investitore ha scelto l'agente: %s\n", chosen);
            

            NODE* current = offers;
            while (current != NULL) {
                if (strcmp(current->item.agentName, chosen) == 0) {
                    if (current->item.quantity > 0) {
                        current->item.revenue += current->item.price;
                        current->item.quantity -= 1;
                        current->item.price += 1;
                    }
                } else {
                    current->item.price -= 1;
                }
                current = current->next;
            }
            
            NODE* chosenNode = NULL;
            NODE* temp = offers;
            while (temp != NULL) {
                if (strcmp(temp->item.agentName, chosen) == 0) {
                    chosenNode = temp;
                    break;
                }
                temp = temp->next;
            }
            if (chosenNode != NULL && chosenNode->item.quantity > 0 &&
                chosenNode->item.price >= chosenNode->item.minPrice) {
                char updateMsg[100];
                sprintf(updateMsg, "Vendita effettuata, ricavo attuale: %d, rimanenti: %d, prezzo attuale: %d",
                        chosenNode->item.revenue, chosenNode->item.quantity, chosenNode->item.price);
                send(chosenNode->item.sock, updateMsg, strlen(updateMsg) + 1, 0);
            }
            
            NODE** curPtr = &offers;
            while (*curPtr != NULL) {
                NODE* node = *curPtr;
                if (node->item.quantity == 0 || node->item.price < node->item.minPrice) {
                    char revenueMsg[50];
                    sprintf(revenueMsg, "Ricavo totale: %d", node->item.revenue);
                    send(node->item.sock, revenueMsg, strlen(revenueMsg) + 1, 0);
                    close(node->item.sock);
                    *curPtr = node->next;
                    free(node);
                } else {
                    curPtr = &((*curPtr)->next);
                }
            }
            
            close(newsockfd);
        } else {
            printf("Tipo di client sconosciuto\n");
            close(newsockfd);
        }
    }
    
    close(sockfd);
    return 0;
}
