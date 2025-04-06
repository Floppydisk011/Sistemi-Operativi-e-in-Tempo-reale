#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>

#define BUF_SIZE 1000
char *host_name = "127.0.0.1";
int port = 8000;

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Uso: %s <nome> <quantità> <prezzo> <prezzoMin>\n", argv[0]);
        exit(1);
    }
    
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
    

    char msg[100];
    snprintf(msg, sizeof(msg), "A %s %s %s %s", argv[1], argv[2], argv[3], argv[4]);
    printf("Registrazione agente: %s\n", msg);
    if (send(sockfd, msg, strlen(msg) + 1, 0) < 0) {
        perror("Errore invio registrazione");
        exit(1);
    }
    
    char buf[BUF_SIZE];
    while (1) {
        memset(buf, 0, BUF_SIZE);
        int n = recv(sockfd, buf, BUF_SIZE, 0);
        if (n < 0) {
            perror("Errore nella ricezione");
            break;
        } else if (n == 0) {
            printf("Connessione chiusa dal server.\n");
            break;
        } else {
            printf("Messaggio ricevuto dal server: %s\n", buf);
        }
    }
    
    close(sockfd);
    return 0;
}
