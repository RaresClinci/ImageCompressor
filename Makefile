CC = gcc
CFLAGS = -g -Wall -lm -std=c99

build: main.c
	$(CC) $(CFLAGS) -o quadtree main.c

clean:
	rm -f quadtree