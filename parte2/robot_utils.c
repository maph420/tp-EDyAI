#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "estr/heap.h"
#include "robot_utils.h"

int aleatorio() {
    srand(time(NULL)) ;
    return (rand() % 2);
}

int infty() {
    return 500;
}

int sum (int a, int b) {
    int inf = infty();
    if (a != inf && b != inf) return a+b;
    return inf;
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

// heurística (distancia Manhattan)
int heuristic(Node a, Node b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Implementación de la función key
Key calcular_key(State s, State start) {
    Key k;
    int min_g_rhs = min(s.g, s.rhs);
    k.key1 = sum(min_g_rhs, heuristic(start.node, s.node));
    k.key2 = min_g_rhs;
    return k;
} 

int g_val(InfoRobot* ir, Node n) {
    return heuristic(n, ir->mapaInterno[ir->j2][ir->i2].node);
}

void imprime_nodo(void* refNodo) {
    State s = *(State*)refNodo;
    fprintf(stderr, "(%d, %d); key: (%d, %d)\n", s.node.x, s.node.y, s.k.key1, s.k.key2);
}

int compara_estado(void* rs1, void* rs2) {
    // (si es el mismo nodo) sirve para la busqueda
    if ( ((*(State*)rs1).node.x == (*(State*)rs2).node.x) &&
    ((*(State*)rs1).node.y == (*(State*)rs2).node.y)) {
        return 2;
    }
    int r;
    Key kA = (*(State*)rs1).k;
    Key kB = (*(State*)rs2).k;
    if (kA.key1 < kB.key1) r = -1;
    if (kA.key1 > kB.key1) r = 1;
    r = (kA.key2 < kB.key2) ? -1 : (kA.key2 > kB.key2) ? 1 : 0;
    
    return r;
}

// para el heap, podria usar simplemente nodos, agregando al tipo de nodo Key
State* crea_estado(Node n, Key k) {
    State* s = malloc(sizeof(State)) ;
    s->node.x = n.x;
    s->node.y = n.y;
    s->k.key1 = k.key1;
    s->k.key2 = k.key2;
    return s;
}

int cost(InfoRobot* ir, State s1, State s2) {
    return (ir->mapaInterno[s1.node.x][s1.node.y].tipoCasilla == OBSTACULO
    || ir->mapaInterno[s2.node.x][s2.node.y].tipoCasilla == OBSTACULO) ? infty() : 1;
}

void impr_mapa(InfoRobot* ir) {
    fprintf(stderr, "(estado, g, rhs)\n");
    for (int i = 0; i < ir->N; i++) {
        for (int j = 0; j < ir->M; j++) {
            fprintf(stderr,"(%d, %d, %d)\t",
            ir->mapaInterno[i][j].tipoCasilla, 
            ir->mapaInterno[i][j].g,
            ir->mapaInterno[i][j].rhs);
        }
        fprintf(stderr, "\n");
    }
}

void inicializa(InfoRobot* ir) {
    // TODO: ver como guardar el rastro del robot, ahora queda medio hardcodeado
    ir->rastro = malloc(sizeof(char) * 150);
    ir->cp = bheap_crear(ir->N * ir->M, compara_estado);
    ir->mapaInterno = malloc(sizeof(State*) * ir->N);

    for (int i = 0; i < ir->N; i++) {
        ir->mapaInterno[i] = malloc(sizeof(State) * ir->M);
        for (int j = 0; j < ir->M; j++) {
            //printf("(%d, %d)\n", i, j);
            ir->mapaInterno[i][j].rhs = infty(); 
            ir->mapaInterno[i][j].g = infty();
            ir->mapaInterno[i][j].tipoCasilla = DESCONOCIDO;
        }
    }
    
    //printf("jijodebu\n");

    ir->mapaInterno[ir->i2][ir->j2].rhs = 0;
    Key k = calcular_key(ir->mapaInterno[ir->i2][ir->j2], ir->mapaInterno[ir->i1][ir->j1]);
    Node nuevoNodo = (Node){ir->i2, ir->j2};
    State* nuevo = crea_estado(nuevoNodo, k);
    ir->cp = bheap_insertar(ir->cp, nuevo);

    //fprintf(stderr, "el heap ahora: \n");
    //bheap_recorrer(ir->cp, imprime_nodo);

    fprintf(stderr, "inicializado\n"); impr_mapa(ir);
}

// todo: cambiar el argumento State por Node (con el node alcanza)
State* obt_ady(InfoRobot* ir, State curr, int* adyCount) {
    State* adyacentes = malloc(sizeof(State) * 4);

    if (curr.node.y < ir-> M - 1) {
        adyacentes[*adyCount].node.x = curr.node.x;
        adyacentes[*adyCount].node.y = curr.node.y + 1;
        (*adyCount)++;
    }
    if (curr.node.y > 0) {
        adyacentes[*adyCount].node.x = curr.node.x;
        adyacentes[*adyCount].node.y = curr.node.y - 1;
        (*adyCount)++;
    } 
    if (curr.node.x < ir-> N - 1) {
        adyacentes[*adyCount].node.x = curr.node.x + 1;
        adyacentes[*adyCount].node.y = curr.node.y;
        (*adyCount)++;
    }
    if (curr.node.x > 0) {
        adyacentes[*adyCount].node.x = curr.node.x - 1;
        adyacentes[*adyCount].node.y = curr.node.y;
        (*adyCount)++;
    }
    return adyacentes;
}

//c(u, s')?
void UpdateVertex(State u, InfoRobot* ir) {
    //fprintf(stderr, "UpdateVertex()\n");
    State* est = crea_estado(u.node, calcular_key(u, ir->mapaInterno[ir->i1][ir->j1]));
    //printf("Current: (%d, %d)\n", est->node.x, est->node.y);

    // (u_x,u_y) != (i2,j2)
    if (u.node.x != ir->i2 || u.node.y != ir->j2) {

        int sucCount = 0;
        State* sucs = obt_ady(ir, u, &sucCount);

        
        int minVal = infty();
        

            //printf("g: %d\n", ir->mapaInterno[sucs[0].node.x][sucs[0].node.y].g);
           /* printf("Sucesor %d: (%d, %d)\n",
            0, 
            sucs[0].node.x,
            sucs[0].node.y
            );*/
        // TODO: poner esto en una funcion
        for (int h = 0; h < sucCount; h++) {

            int v = sum(cost(ir, ir->mapaInterno[sucs[h].node.x][sucs[h].node.y], 
            u),
            ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].g);

            if (v < minVal) {
                minVal = v;
            }
        }
        // siguiente nodo a recorrer
        ir->mapaInterno[u.node.x][u.node.y].rhs = minVal;
        est->k = calcular_key(ir->mapaInterno[u.node.x][u.node.y], ir->mapaInterno[ir->i1][ir->j1]);
        free(sucs);
    } 

    //printf("Se ejecuta buscar y elim (%d, %d)\n", u.node.x, u.node.y);
    int e = bheap_buscar_eliminar(ir->cp, &u);
    // fprintf(stderr, "%s\n", e>0? "Se encontro y elimino" : "No encontrado");

    // si el nodo no es consistente, agregar al heap
    if (ir->mapaInterno[u.node.x][u.node.y].rhs != 
    ir->mapaInterno[u.node.x][u.node.y].g) {
     //   fprintf(stderr, "Insertado en el heap\n");
        ir->cp = bheap_insertar(ir->cp, est);
    }

}

int comp_key(Key kA, Key kB) {
    if (kA.key1 < kB.key1) return -1;
    if (kA.key1 > kB.key1) return 1;
    return (kA.key2 < kB.key2) ? -1 : (kA.key2 > kB.key2) ? 1 : 0;
}

void ComputeShortestPath(InfoRobot* ir, Node start) {
    //fprintf(stderr, "ComputeShortestPath()\n");
    int count = 0;

// Pred(curr): conj casillas desde las cuales puedo llegar a curr
// Suc(curr): conj casillas a las que puedo llegar desde curr
// 4 puntos cardinales, impedimentos? obstaculo en alguna direccion.

    //printf("Llave tope: (%.1lf, %.1lf)\n", llaveTope.key1, llaveTope.key2)

// while (U.TopKey() < CalculateKey(s_start) OR rhs(s_start) != g(s_start))
    while (((!bheap_es_vacio(ir->cp) && (comp_key(((State*)bheap_minimo(ir->cp))->k, 
    calcular_key(ir->mapaInterno[start.x][start.y], ir->mapaInterno[start.x][start.y]))) < 0)) 
    || (ir->mapaInterno[start.x][start.y].rhs) != (ir->mapaInterno[start.x][start.y].g)) {
        

        //fprintf(stderr, "----\nMAPA:\n----\n"); impr_mapa(ir);

        //fprintf(stderr, "----\nBHEAP:\n----\n"); bheap_recorrer(ir->cp, imprime_nodo);
        
        // popear el maximo elemento de la cola
        State u = *(State*)bheap_minimo(ir->cp);
        ir->cp = bheap_eliminar_minimo(ir->cp);
        //fprintf(stderr, "minimo: %d,%d\n", u.node.x, u.node.y);

        // no descubierto
        if (ir->mapaInterno[u.node.x][u.node.y].g > 
        ir->mapaInterno[u.node.x][u.node.y].rhs) {
        //fprintf(stderr, "Es sobreconsistente (g > rhs). Poner g = rhs\n");
            ir->mapaInterno[u.node.x][u.node.y].g = ir->mapaInterno[u.node.x][u.node.y].rhs;
            int cantPred = 0;
            State* pred = obt_ady(ir, u, &cantPred);
            for (int h = 0; h < cantPred; h++) {
                UpdateVertex(pred[h], ir);
            }
        } 
        else {
            //fprintf(stderr, "No es sobreconsistente (g <= rhs), poner g = inf\n");
            ir->mapaInterno[u.node.x][u.node.y].g = infty();
           
            UpdateVertex(u, ir);
            int cantPred = 0;
            State* pred = obt_ady(ir, u, &cantPred);
            for (int h = 0; h < cantPred; h++) {
                UpdateVertex(pred[h], ir);
            }
            
    }

  
     /*printf("rhs ini %d, g ini %d\n",
        ir->mapaInterno[ir->j1][ir->i1].rhs,
        ir->mapaInterno[ir->j1][ir->i1].g) ;*/
       //llave = calcular_key(ir->mapaInterno[ir->i1][ir->j1], ir->mapaInterno[ir->i1][ir->j1]);
        //printf("Key inicial: (%d, %d)\n", llave.key1, llave.key2);
        //printf("BHeap ahora: \n"); bheap_recorrer(ir->cp, imprime_nodo);
}
//printf("sale del while\n");
//printf("El mapa queda:\n"); impr_mapa(ir);
}

void actualizar_segun_direccion(InfoRobot* ir, State sig, int dist, int dx, int dy, int* o, int multiplesOpciones) {
    
    //fprintf(stderr, "dist: %d, dmax: %d, dx: %d, dy: %d\n", dist, ir->d_max, dx, dy);
    // detecto obstaculo
    if (dist <= ir->distSensorConocida) {

        /*fprintf(stderr, "%d %d\n",
        (ir->x + (dist * dx)),
        (ir->y + (dist * dy))
        );*/

        if ( (ir->y + (dist * dy)) >= 0 && (ir->y + (dist * dy)) < (ir->M) &&
        (ir->x + (dist * dx)) >= 0 && (ir->x + (dist * dx)) < (ir->N ) ) {
            ir->mapaInterno[ir->x + (dist * dx)][ir->y + (dist * dy)].tipoCasilla = OBSTACULO;
            (*o)++;
            fprintf(stderr,"obstaculo detectado\n");
            int indXObstaculo = ir->x + (dist * dx);
            int indYObstaculo = ir->y + (dist * dy);
            Node n = (Node){indXObstaculo, indYObstaculo};
            ir->mapaInterno[indXObstaculo][indYObstaculo].node = n;

            ir->mapaInterno[indXObstaculo][indYObstaculo].rhs = infty();

            fprintf(stderr, "casilla: %d,%d\n", ir->mapaInterno[indXObstaculo][indYObstaculo].node.x, ir->mapaInterno[indXObstaculo][indYObstaculo].node.y);


            UpdateVertex(ir->mapaInterno[indXObstaculo][indYObstaculo], ir);
            int contAdyacentes = 0;
            State* ady = obt_ady(ir, ir->mapaInterno[indXObstaculo][indYObstaculo], &contAdyacentes);
            for (int h = 0; h < contAdyacentes; h++) {
                UpdateVertex(ady[h], ir);
            }
            ComputeShortestPath(ir, ir->mapaInterno[indXObstaculo][indYObstaculo].node);
            fprintf(stderr, "el robot scanea desde %d,%d\n",ir->x, ir->y);
            fprintf(stderr, "dx: %d dy: %d\n", dx, dy);
            fprintf(stderr, "cambia %d, %d\n", ir->x + dx, ir->y + dy);
            if (*o == 2 && multiplesOpciones) {
                (ir->mapaInterno[ir->x + dx][ir->y + dy].g) ++;
            }

        }
        
    }
    for (int h = 1; h < dist; h++) {
        if (ir->mapaInterno[ir->x + (h * dx)][ir->y + (h * dy)].tipoCasilla == DESCONOCIDO)
            ir->mapaInterno[ir->x + (h * dx)][ir->y + (h * dy)].tipoCasilla = VALIDO;
    }
}

void actualizar_mapa_interno(InfoRobot* ir, int* d, State siguiente, int m) {
    int cantObstaculos = 0;
    actualizar_segun_direccion(ir, siguiente, d[0], -1, 0, &cantObstaculos, m); // arriba
    actualizar_segun_direccion(ir, siguiente, d[1], 1, 0, &cantObstaculos, m); // abajo
    actualizar_segun_direccion(ir, siguiente, d[2], 0, -1, &cantObstaculos, m); // izq
    actualizar_segun_direccion(ir, siguiente, d[3], 0, 1, &cantObstaculos, m); // der
}

int siguiente_movimiento(InfoRobot* ir, int currX, int currY, State* posibles) {
    int sucCount = 0, posiblesMovimientos = 0;
    State curr; curr.node.x = currX ; curr.node.y = currY;
    State* sucs = obt_ady(ir, curr, &sucCount);

    posibles[0] = sucs[0];
    int minVal = infty();

    for (int h = 0; h < sucCount; h++) {
         
        int v = sum(cost(ir, ir->mapaInterno[sucs[h].node.x][sucs[h].node.y], 
        ir->mapaInterno[curr.node.x][curr.node.y]),
        ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].g);

        if (v < minVal) {
            minVal = v;
            posibles[0] = sucs[h];
            posiblesMovimientos = 1;
        } else if (v == minVal) {
            posibles[1] = sucs[h];
            posiblesMovimientos = 2;
        }
    }
    free(sucs);
    fprintf(stderr, "Proximo mov -> (%d, %d)\n", posibles[0].node.x, posibles[0].node.y);
    
    if (posiblesMovimientos == 2) fprintf(stderr, "o... (%d, %d)\n", posibles[1].node.x, posibles[1].node.y);
    else fprintf(stderr, "(solo ese)\n");
    return posiblesMovimientos;
}

// se asume que (ir->x, ir->y) != (sig.x, sig.y)
int mover_robot(InfoRobot* ir, Node sig, int pasos) {
    int difX = ir->x - sig.x;
    if (difX > 0) {
        ir->x--; //arriba
        ir->rastro[pasos] = 'U';
    } else if (difX < 0) {
        ir->x ++; //abajo
        ir->rastro[pasos] = 'D';
    } else {
        int difY = ir->y - sig.y;
        if (difY > 0) {
            ir->y--; // izquierda
            ir->rastro[pasos] = 'L';
        } else {
            ir->y++; // derecha
            ir->rastro[pasos] = 'R';
        }
    }
    return ++pasos;
}
