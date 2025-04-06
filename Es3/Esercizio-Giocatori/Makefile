all: client server

client: client.c
	$(CC) $(CFLAGS) -o client client.c

server: server.c list.c list.h
	$(CC) $(CFLAGS) -o server server.c list.c

clean:
	rm -f client server *~