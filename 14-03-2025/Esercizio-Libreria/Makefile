all: server publisher reader

server: server.c list.c list.h
	$(CC) $(CFLAGS) -o server server.c list.c

publisher: pub.c
	$(CC) $(CFLAGS) -o pub pub.c

reader: red.c
	$(CC) $(CFLAGS) -o red red.c

clean:
	rm -f server pub red *~