all:	agent investor server

agent:	agent.c
	gcc -Wall -o agent agent.c

investor:	investor.c
	gcc -Wall -o investor investor.c

server:	server.c list.c
	gcc -Wall -o server server.c list.c

clean:
	rm -f agent investor server *~