#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUF_SIZE 1000
char *host_name = "127.0.0.1"; /* localhost */
int port = 8000;

int main(int argc, char *argv[]) {
    char str[50] = "Giocatore_default";
    if (argc < 2) {
        printf("Usage: %s \"nome_giocatore\"\nUtilizzo del nome di default.\n", argv[0]);
    } else {
        strncpy(str, argv[1], sizeof(str) - 1);
    }
    
    struct sockaddr_in serv_addr;
    struct hostent* server;    
    
    if ((server = gethostbyname(host_name)) == NULL) {
        perror("Errore nella risoluzione dell'host");
        exit(1);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr))->s_addr;
    serv_addr.sin_port = htons(port);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Errore apertura socket");
        exit(1);
    }
    
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Errore connessione");
        exit(1);
    }
    
    printf("Invio del nome \"%s\" al server...\n", str);
    if (send(sockfd, str, strlen(str)+1, 0) < 0) {
        perror("Errore in invio");
        exit(1);
    }
    
    printf("Messaggio inviato. In attesa del risultato...\n");
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    if (recv(sockfd, buf, BUF_SIZE, 0) < 0) {
        perror("Errore in ricezione");
        exit(1);
    }
    
    printf("\nRisultato dal server:\n%s\n", buf);
    close(sockfd);
    
    return 0;
}
