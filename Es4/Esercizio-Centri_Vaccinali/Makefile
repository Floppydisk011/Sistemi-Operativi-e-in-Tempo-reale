CC = gcc
CFLAGS = -Wall

all: server supplier center

server: server.o list.o
	$(CC) $(CFLAGS) -o server server.o list.o

supplier: supplier.o list.o
	$(CC) $(CFLAGS) -o supplier supplier.o list.o

center: center.o list.o
	$(CC) $(CFLAGS) -o center center.o list.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o server supplier center *~
