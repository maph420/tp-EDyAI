OBJS = robot.o robot_utils.o estr/bheap.o
SOURCE = robot.c robot_utils.c estr/bheap.c
HEADER = robot_utils.h estr/bheap.h
OUT	= robot
FLAGS = -g -c -Wall -Wextra -Werror
CC = gcc

all:	robot

robot: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c $(HEADER)
	$(CC) $(FLAGS) -c -o $@ $< $(LFLAGS)

clean:
	rm -f $(OBJS) $(OUT)

# evita errores de compatibilidad, en caso de tener un
# archivo llamado clean
.PHONY = clean