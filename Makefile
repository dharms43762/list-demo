CC=gcc
CFLAGS = -g

list-demo: list-demo.o list.o
	gcc list-demo.o list.o -o list-demo

clean:
	rm list-demo *.o
