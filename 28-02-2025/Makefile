main: main.o libfasttrigo.a
	gcc -o main main.o -L. -lfasttrigo -lm

libfasttrigo.a: fast_trigo.o
	ar -rc libfasttrigo.a fast_trigo.o

main.o: main.c fast_trigo.h
	gcc -c main.c

fast_trigo.o: fast_trigo.c fast_trigo.h
	gcc -c fast_trigo.c
