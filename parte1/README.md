# tp-EDyAI

Approach 1: robot random "inteligente". Ir en la (las) direcciones "preferidas" e ir variando de manera aleatoria cada movimiento. La decision del robot de cual sera la siguiente casilla se basara en: la o las direcciones favoritas. Si la que escoge el robot es obstaculo, se vuelve para atras o si ya se visito, se intenta otra alternativa. Cuando ya no le quedan alternativas, debe volver hacia atras hasta encontrar una casilla que tenga casillas adyacentes que verifiquen la elejibilidad del robot.

Movimientos del robot -> pila (glist)
Casilleros ya visitados + obstaculos -> AVL, nodo con (posX, posY, valida) : (int, int, int)
Metodo de orden de los nodos: lexicografico

Approach 2: similar al 1 pero tratar de mejorar eficiencia eligiendo direccion cuando sea estrictamente necesario, es decir, cuando se encuentra un obstaculo, de lo contrario ir siempre en una direccion fija

Compilar y correr parte 1:

> gcc robot.c robot_utils.c estr/avl.c estr/pila.c estr/glist.c -Wall -Wextra -std=c99 -g -o parte1

> ./parte1 ejemplos/ejemplo8.txt

Debug:

> valgrind --leak-check=full -s ./parte1 mazes/maze_2.txt

Test parte 1:

> ./testing_script.sh 15 15 100

Otro:

> gcc otro.c -Wall -Wextra -std=c99 -g -lm -o otro

Todo:

- [ ] Ver si vale la pena mejorar la inteligencia al backtrackear (si no se pierde mucha eficiencia, y si acorta el camino bastante). En el ejemplo.txt, si se arranca por abajo se pueden hacer hasta 29 pasos, mientras que la ruta ideal es de 11.

- [ ] Testear con mapas mas retorcidos

- [ ] Seguir mejorando calidad de codigo, va queriendo. e.g. se declaran muchas variables nuevas en cada vuelta, ver como mejorar eso

- [ ] Ver si vale la pena la utilizacion del arbol

- [ ] Medir eficiencia