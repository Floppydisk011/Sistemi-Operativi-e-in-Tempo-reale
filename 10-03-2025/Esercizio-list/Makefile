CC = gcc
CFLAGS = -g -Wall

all: prova

prova: prova.o list.o
	$(CC) $(CFLAGS) -o prova prova.o list.o

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f prova *.o *~

