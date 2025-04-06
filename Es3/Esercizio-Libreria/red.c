#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUF_SIZE 1000
#define PORT 8000
char *host_name = "127.0.0.1";

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <titolo_libro>\n", argv[0]);
        exit(1);
    }
    char title[21];
    strncpy(title, argv[1], 20);
    title[20] = '\0';
    
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Errore apertura socket");
        exit(1);
    }
    
    server = gethostbyname(host_name);
    if (server == NULL) {
        fprintf(stderr, "Host non trovato\n");
        exit(1);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);
    
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Errore connessione");
        exit(1);
    }
    
    char msg[BUF_SIZE];
    snprintf(msg, BUF_SIZE, "L:%s", title);
    if (send(sockfd, msg, strlen(msg) + 1, 0) < 0) {
        perror("Errore invio");
        exit(1);
    }
    
    printf("Richiesta inviata per il libro \"%s\". In attesa di risposta...\n", title);
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    if (recv(sockfd, buf, BUF_SIZE, 0) < 0) {
        perror("Errore ricezione");
        exit(1);
    }
    printf("Risposta dal server: %s\n", buf);
    close(sockfd);
    return 0;
}
