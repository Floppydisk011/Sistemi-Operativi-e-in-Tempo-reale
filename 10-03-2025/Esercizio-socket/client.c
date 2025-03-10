#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/wait.h>

#define NUM_CHILDS 5

char *host_name = "127.0.0.1";
int port = 8000;

void child_process() {
    struct sockaddr_in serv_addr;
    struct hostent* server;	
    int num, answer;

    srand(time(NULL) + getpid()); // seed unico per ogni processo figlio
    num = rand() % 100;           // numero casuale unico generato dal figlio

    if ((server = gethostbyname(host_name)) == NULL) {
        perror("Error resolving local host");
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr))->s_addr;
    serv_addr.sin_port = htons(port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error opening socket");
        exit(1);
    }    

    if (connect(sockfd, (void*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Error connecting to socket");
        exit(1);
    }

    printf("[Child %d] Sending number %d\n", getpid(), num);

    if (send(sockfd, &num, sizeof(num), 0) == -1) {
        perror("Error on send");
        exit(1);
    }

    printf("[Child %d] Waiting response...\n", getpid());

    if (recv(sockfd, &answer, sizeof(answer), 0) == -1) {
        perror("Error in receiving response from server");
        exit(1);
    }

    printf("[Child %d] Received response: %d\n", getpid(), answer);

    close(sockfd);
    exit(0);
}

int main(int argc, char *argv[]) {
    for (int i = 0; i < NUM_CHILDS; i++) {
        if (fork() == 0) {
            child_process();
        }
    }

    // Il padre aspetta che tutti i figli finiscano
    for (int i = 0; i < NUM_CHILDS; i++)
        wait(NULL);

    return 0;
}
