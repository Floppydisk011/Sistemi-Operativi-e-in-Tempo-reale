#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>


#define BUF_SIZE 1000


int port = 8000;


int main() 
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	int num, ret;

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
	
	// Maximum number of connection kept in the socket queue
	if ( listen( sockfd, 20 ) == -1 ) 
	{
		perror("Error on listen");
		exit(1);
	}


	socklen_t address_size = sizeof( cli_addr );	

	while(1) 
	{
		printf("\n---\npid %d: waiting connections\n", getpid());
		
		// New connection acceptance		
		int newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
		if (newsockfd == -1) 
		{
			perror("Error on accept");
			exit(1);
		}
		
		ret = fork();
		
		if (ret == 0) {
		  printf("pid %d child of %d: serving active connection\n", getpid(), getppid());
		  // Message reception
		  if (recv(newsockfd, &num, sizeof(num), 0 ) == -1) {
		    perror("Error on receive");
		    exit(1);
		  }
		  printf("pid %d: received %d; respond incrementing by 1 \n", getpid(), num);
		  num++;
		  if (send(newsockfd, &num, sizeof(num), 0 ) == -1) {
		    perror("Error on send");
		    exit(1);
	          }
		  printf("pid %d: close the connections and exit\n",getpid());
		  close(newsockfd);
		  exit(0);
		}
		else {
		  printf("pid %d: generated the process %d: continue executions serving other clients\n", getpid(), ret);
		  close(newsockfd);
		}
	}

	close(sockfd);
	return 0;
}



