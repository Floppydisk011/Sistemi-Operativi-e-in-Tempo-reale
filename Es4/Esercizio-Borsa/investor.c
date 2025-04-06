#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>

#define BUF_SIZE 1000
char *host_name = "127.0.0.1";
int port = 8000;

int main(int argc, char *argv[]) {
    
    struct sockaddr_in serv_addr;
    struct hostent* server;
    if ((server = gethostbyname(host_name)) == NULL) {
        perror("Errore risolvendo host");
        exit(1);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr))->s_addr;
    serv_addr.sin_port = htons(port);
    
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Errore apertura socket");
        exit(1);
    }
    
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Errore connessione");
        exit(1);
    }
    
    char identifier[10] = "I\n";
    if (send(sockfd, identifier, strlen(identifier) + 1, 0) < 0) {
        perror("Errore invio identificatore investitore");
        exit(1);
    }
    
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    int n = recv(sockfd, buf, BUF_SIZE, 0);
    if (n <= 0) {
        perror("Errore ricezione lista");
        close(sockfd);
        exit(1);
    }
    printf("Lista delle quotazioni:\n%s\n", buf);
    
    if (strncmp(buf, "Nessuna offerta registrata", 27) == 0) {
        printf("Non ci sono offerte disponibili. Uscita.\n");
        close(sockfd);
        return 0;
    }
    
    char chosen[31];
    printf("Inserisci l'agente da cui acquistare: ");
    if (fgets(chosen, sizeof(chosen), stdin) == NULL) {
        printf("Errore nella lettura dell'input.\n");
        close(sockfd);
        exit(1);
    }
    chosen[strcspn(chosen, "\n")] = '\0'; 

    if (send(sockfd, chosen, strlen(chosen) + 1, 0) < 0) {
        perror("Errore invio nome agente scelto");
        exit(1);
    }
    
    close(sockfd);
    return 0;
}
