all: server guide visitor

server: server.c list.c list.h
	gcc -Wall -o server server.c list.c

guide: guide.c
	gcc -Wall -o guide guide.c

visitor: visitor.c
	gcc -Wall -o visitor visitor.c

clean:
	rm -f server guide visitor *~
