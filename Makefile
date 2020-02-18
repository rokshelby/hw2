CC = gcc -g

all: oss prime 

oss: oss.o
	$(CC) oss.c -o oss
prime: prime.o
	$(CC) prime.c -o prime
oss.o: oss.c
	$(CC) oss.c
prime.o: prime.c
	$(CC) prime.c
clean:
	rm -rf *.o

