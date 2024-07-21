# tp-EDyAI

Approach 1: robot random "inteligente". Ir en la (las) direcciones "preferidas" e ir variando de manera aleatoria cada movimiento. La decision del robot de cual sera la siguiente casilla se basara en: la o las direcciones favoritas. Si la que escoge el robot es obstaculo, se vuelve para atras o si ya se visito, se intenta otra alternativa. Cuando ya no le quedan alternativas, debe volver hacia atras hasta encontrar una casilla que tenga casillas adyacentes que verifiquen la elejibilidad del robot.

Movimientos del robot -> pila (glist)
Casilleros ya visitados + obstaculos -> AVL, nodo con (posX, posY, valida) : (int, int, int)
Metodo de orden de los nodos: lexicografico

Approach 2: similar al 1 pero tratar de mejorar eficiencia eligiendo direccion cuando sea estrictamente necesario, es decir, cuando se encuentra un obstaculo, de lo contrario ir siempre en una direccion fija

Compilar y correr parte 1:

> gcc func.c estr/avl.c estr/pila.c estr/glist.c -Wall -Wextra -std=c99 -o parte1

> ./parte1 ejemplo2.txt

Todo:

. Agregar repe de nodos

. Solucionar backtracking (casi siempre se queda atrapado y no reconoce que debe hacer backtrack, con la repe de nodos seguro va)

. Cambiar registro de robot (no debe moverse el robot a celdas que NO puede, tiene que consultar siempre ANTES, eso se interpreta)

. Mejorar la calidad del codigo (un espanto, demasiado if, seguro hay cosas innecesarias)
