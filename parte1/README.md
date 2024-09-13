# tp-EDyAI

Compilar y correr parte 1:

> make

Alternativamente:

> gcc main.c robot_utils.c estr/pila.c estr/glist.c estr/tablahash.c -Wall -Wextra -std=c99 -g -o parte1

> ./parte1 ejemplos/ejemplo.txt

Debug:

> valgrind --leak-check=full -s ./parte1 mazes/maze_2.txt

Test parte 1:

> ./testing_script.sh 15 15 100