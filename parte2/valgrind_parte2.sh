#!/bin/sh

# Parar y eliminar cualquier proceso anterior
pkill -f sensor
pkill -f programa

# Compilar el programa principal utilizando el Makefile
gcc robot.c robot_utils.c estr/bheap.c -lm -Wall -Werror -std=c99 -o robot

# Compilar el programa sensor
make sensor -s

# Crear fifo en /tmp
fifo="/tmp/feedback_fifo"
mkfifo $fifo

# Ejecutar los programas en paralelo utilizando el fifo con valgrind
(valgrind --leak-check=full --track-origins=yes --verbose ./robot < $fifo | ./sensor $1 > $fifo) &

# Asegurarse de que ambos programas han terminado
wait

# Eliminar fifo
rm $fifo