OBJS = main.o estr/glist.o estr/pila.o estr/tablahash.o robot_utils.o
SOURCE = main.c estr/glist.c estr/pila.c estr/tablahash.c robot_utils.c
HEADER = glist.h estr/pila.h estr/tablahash.h robot_utils.h
OUT	= parte1
FLAGS = -g -c -Wall
CC = gcc

all:	parte1

parte1: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c $(HEADER)
	$(CC) $(FLAGS) -c -o $@ $< $(LFLAGS)

clean:
	rm -f $(OBJS) $(OUT)

# evita errores de compatibilidad, en caso de tener un
# archivo llamado clean
.PHONY = clean