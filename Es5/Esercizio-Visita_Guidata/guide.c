#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>


#define BUF_SIZE 1000


char *host_name = "127.0.0.1"; /* local host */
int port = 8000;


int main(int argc, char *argv[]) 
{
	if(argc != 4) {
        fprintf(stderr, "Usage: %s <nome> <min> <max>\n", argv[0]);
        exit(1);
    }
    char *nome = argv[1];
    char *min = argv[2];
    char *max = argv[3];

    char msg[BUF_SIZE];
    snprintf(msg, BUF_SIZE, "G;%s;%s;%s", nome, min, max);
	struct sockaddr_in serv_addr;
 	struct hostent* server;	
	
	if ( ( server = gethostbyname(host_name) ) == 0 ) 
	{
		perror("Error resolving local host\n");
		exit(1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr))->s_addr;
	serv_addr.sin_port = htons(port);
	
	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) 
	{
		perror("Error opening socket\n");
		exit(1);
	}    

	if ( connect(sockfd, (void*)&serv_addr, sizeof(serv_addr) ) == -1 ) 
	{
		perror("Error connecting to socket\n");
		exit(1);
	}


	printf("Sending message \"%s\" to server...\n", msg);

	/* This sends the string plus the string terminator '\0' */
	if ( send(sockfd, msg, strlen(msg)+1, 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("Message sent. Waiting for response...\n");
	
	char buf[BUF_SIZE];	
	buf[0]='\0';

	if ( recv(sockfd, buf, BUF_SIZE, 0) == -1 ) 
	{
		perror("Error in receiving response from server\n");
		exit(1);
	}

    printf("Guida %s: Visita avviata con %s visitatori.\n", nome, buf);

	close(sockfd);

	return 0;
}



