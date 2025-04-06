all: server supplier center

server: server.c list.c
	gcc -Wall -o server server.c list.c

supplier: supplier.c list.c
	gcc -Wall -o supplier supplier.c list.c

center: center.c list.c
	gcc -Wall -o center center.c list.c

clean:
	rm -f server supplier center *~