CC = gcc
CFLAGS = -Wall -pedantic -ansi -std=c99
OBJ = os.o list.o queue.o

kek : $(OBJ)
	$(CC) -o kek $(OBJ)

os.o : os.c list.h queue.h
	$(CC) $(CFLAGS) -c os.c

list.o : list.c list.h
	$(CC) $(CFLAGS) -c list.c

queue.o : queue.c queue.h os.h
	$(CC) $(CFLAGS) -c queue.c

clean:
	rm -f kek $(OBJ)