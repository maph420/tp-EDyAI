# tp-EDyAI

Approach 1: robot random "inteligente". Ir en la (las) direcciones "preferidas" e ir variando de manera aleatoria cada movimiento. La decision del robot de cual sera la siguiente casilla se basara en: la o las direcciones favoritas. Si la que escoge el robot es obstaculo, se vuelve para atras o si ya se visito, se intenta otra alternativa. Cuando ya no le quedan alternativas, debe volver hacia atras hasta encontrar una casilla que tenga casillas adyacentes que verifiquen la elejibilidad del robot.

Movimientos del robot -> pila (glist)
Casilleros ya visitados + obstaculos -> AVL, nodo con (posX, posY, valida) : (int, int, int)
Metodo de orden de los nodos: lexicografico

Compilar y correr parte 1:

> gcc parte1.c avl.c -Wall -Wextra -std=c99 -o parte1

> ./parte1 ejemplo2.txt