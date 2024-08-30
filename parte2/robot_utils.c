#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "robot_utils.h"

// retorna 0 o 1 de manera aleatoria
int aleatorio() {
    srand(time(NULL)) ;
    return (rand() % 2);
}

int infty() {
    return 5000;
}

// suma en base al infinito designado
int suma_inf (int a, int b) {
    int inf = infty();
    if (a != inf && b != inf) return a+b;
    return inf;
}

int max(int* l, int tam) {
    int maxElem = l[0];
    for(int i = 0; i < tam; i++) {
        if (l[i] > maxElem) maxElem = l[i];
    }
    return maxElem;
}

// heurística (distancia Manhattan)
int heuristica(Nodo a, Nodo b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Implementación de la función key
Key obt_key(State s, State start) {
    int min_g_rhs =  (s.g < s.rhs) ? s.g : s.rhs;
    return (Key){suma_inf(min_g_rhs, heuristica(start.nodo, s.nodo)), min_g_rhs};
} 

int g_val(InfoRobot* ir, Nodo n) {
    return heuristica(n, ir->mapaInterno[ir->i2][ir->j2].nodo);
}

void imprime_nodo(void* refNodo) {
    //fprintf(stderr,"impr nodo\n");
    if (refNodo == NULL)
        fprintf(stderr,"es null\n");
    else {
    EstadoConClave sk = *(EstadoConClave*)refNodo;
    fprintf(stderr, "(%d, %d); key: (%d, %d)\n", sk.est.nodo.x, sk.est.nodo.y, sk.key.id_1, sk.key.id_2);
    }
}

int comp_keys(Key kA, Key kB) {
    return (kA.id_1 < kB.id_1) ? -1 : 
    (kA.id_1 > kB.id_1) ? 1 :
    (kA.id_2 < kB.id_2) ? -1 : 
    (kA.id_2 > kB.id_2) ? 1 : 
    0;
}

int compara_estado_con_clave(void* sk1, void* sk2) {
    if ( ((*(EstadoConClave*)sk1).est.nodo.x == (*(EstadoConClave*)sk2).est.nodo.x) &&
    ((*(EstadoConClave*)sk1).est.nodo.y == (*(EstadoConClave*)sk2).est.nodo.y)) {
        return 2;
    }
    Key kA = (Key){(*(EstadoConClave*)sk1).key.id_1, (*(EstadoConClave*)sk1).key.id_2};
    Key kB = (Key){(*(EstadoConClave*)sk2).key.id_1, (*(EstadoConClave*)sk2).key.id_2};
    return comp_keys(kA, kB);
}

// para el heap, podria usar simplemente nodos, agregando al tipo de nodo Key
EstadoConClave* crea_estado_con_clave(Nodo n, Key k, int g, int rhs, int estadoCasillero) {
    EstadoConClave* sk = malloc(sizeof(EstadoConClave));
    sk->est.nodo = (Nodo){n.x, n.y};
    sk->key = (Key){k.id_1, k.id_2};
    sk->est.g = g;
    sk->est.rhs = rhs;
    sk->est.tipoCasilla = estadoCasillero;
    return sk;
}

void* copia_est_con_clave(void* s) {
    EstadoConClave* sk = malloc(sizeof(EstadoConClave));
    sk->est.nodo = (Nodo){((EstadoConClave*)s)->est.nodo.x, ((EstadoConClave*)s)->est.nodo.y};
    sk->key = (Key){((EstadoConClave*)s)->key.id_1,((EstadoConClave*)s)->key.id_2};
    sk->est.g = ((EstadoConClave*)s)->est.g;
    sk->est.rhs = ((EstadoConClave*)s)->est.rhs;
    sk->est.tipoCasilla = ((EstadoConClave*)s)->est.tipoCasilla;
    return sk;
}

void destruir_est_con_clave(void* s) {
    free((EstadoConClave*)s);
}

// El costo entre dos estados es infinito si alguno es un obstaculo 
// (imposible moverse entre ellos). En otro caso, es 1.
int costo_movimiento(InfoRobot* ir, State s1, State s2) {
    return (ir->mapaInterno[s1.nodo.x][s1.nodo.y].tipoCasilla == OBSTACULO
    || ir->mapaInterno[s2.nodo.x][s2.nodo.y].tipoCasilla == OBSTACULO) ? infty() : 1;
}


void impr_mapa(InfoRobot* ir) {
    //fprintf(stderr, "(estado, g, rhs)\n");
    for (int i = 0; i < ir->N; i++) {
        for (int j = 0; j < ir->M; j++) {
            fprintf(stderr,"(%d, %d, %d)",
            ir->mapaInterno[i][j].tipoCasilla, 
            ir->mapaInterno[i][j].g,
            ir->mapaInterno[i][j].rhs);
            if (i == ir->x && j == ir->y) 
                fprintf(stderr, "*\t");
            if (i == ir->i2 && j == ir->j2)
                fprintf(stderr, "!\t");
            else fprintf(stderr, "\t");
        }
        fprintf(stderr, "\n");
    }
}

void inicializa(InfoRobot* ir) {
    ir->rastro = malloc(sizeof(char) * 150);
    ir->cp = bheap_crear(ir->N * ir->M, compara_estado_con_clave, destruir_est_con_clave, copia_est_con_clave);
    ir->mapaInterno = malloc(sizeof(State*) * ir->N);
    // ubicar al robot en la pos inicial dada
    ir->x = ir->i1; ir->y = ir->j1;

    for (int i = 0; i < ir->N; i++) {
        ir->mapaInterno[i] = malloc(sizeof(State) * ir->M);
        for (int j = 0; j < ir->M; j++) {
            ir->mapaInterno[i][j].rhs = infty(); 
            ir->mapaInterno[i][j].g = infty();
            ir->mapaInterno[i][j].tipoCasilla = DESCONOCIDO;
            ir->mapaInterno[i][j].nodo.x = i;
            ir->mapaInterno[i][j].nodo.y = j;
        }
    }

    //printf("jijodebu\n");
    ir->mapaInterno[ir->i1][ir->j1].tipoCasilla = VISITADO;
    ir->mapaInterno[ir->i2][ir->j2].rhs = 0;

    EstadoConClave estadoMeta = {ir->mapaInterno[ir->i2][ir->j2], 
    obt_key(ir->mapaInterno[ir->i2][ir->j2], ir->mapaInterno[ir->i1][ir->j1])};
    
    ir->cp = bheap_insertar(ir->cp, &estadoMeta);
}

// todo: cambiar el argumento State por Nodo (con el Nodo alcanza)
State* obt_ady(InfoRobot* ir, State curr, int* adyCount) {
    State* adyacentes = malloc(sizeof(State) * 4);
    // malloc en cada vuelta
    if (curr.nodo.y < ir-> M - 1) {
        adyacentes[*adyCount].nodo.x = curr.nodo.x;
        adyacentes[*adyCount].nodo.y = curr.nodo.y + 1;
        adyacentes[*adyCount].g = ir->mapaInterno[curr.nodo.x][curr.nodo.y + 1].g;
        adyacentes[*adyCount].rhs = ir->mapaInterno[curr.nodo.x][curr.nodo.y + 1].rhs;
        (*adyCount)++;
    }
    if (curr.nodo.y > 0) {
        adyacentes[*adyCount].nodo.x = curr.nodo.x;
        adyacentes[*adyCount].nodo.y = curr.nodo.y - 1;
        adyacentes[*adyCount].g = ir->mapaInterno[curr.nodo.x][curr.nodo.y - 1].g;
        adyacentes[*adyCount].rhs = ir->mapaInterno[curr.nodo.x][curr.nodo.y - 1].rhs;
        (*adyCount)++;
    } 
    if (curr.nodo.x < ir-> N - 1) {
        adyacentes[*adyCount].nodo.x = curr.nodo.x + 1;
        adyacentes[*adyCount].nodo.y = curr.nodo.y;
        adyacentes[*adyCount].g = ir->mapaInterno[curr.nodo.x +1][curr.nodo.y ].g;
        adyacentes[*adyCount].rhs = ir->mapaInterno[curr.nodo.x +1][curr.nodo.y ].rhs;
        (*adyCount)++;
    }
    if (curr.nodo.x > 0) {
        adyacentes[*adyCount].nodo.x = curr.nodo.x - 1;
        adyacentes[*adyCount].nodo.y = curr.nodo.y;
        adyacentes[*adyCount].g = ir->mapaInterno[curr.nodo.x -1][curr.nodo.y].g;
        adyacentes[*adyCount].rhs = ir->mapaInterno[curr.nodo.x -1][curr.nodo.y].rhs;
        (*adyCount)++;
    }
    return adyacentes;
}

void UpdateVertex(State u, InfoRobot* ir) {
    fprintf(stderr, "updatevertex: %d,%d\n", u.nodo.x, u.nodo.y);

    EstadoConClave sk = {u, obt_key(u, ir->mapaInterno[ir->x][ir->y])};

    // (u_x,u_y) != (i2,j2)
    if (u.nodo.x != ir->i2 || u.nodo.y != ir->j2) {

        int sucCount = 0;
        State* sucs = obt_ady(ir, u, &sucCount);

        int minVal = infty();
        for (int h = 0; h < sucCount; h++) {

            int v = suma_inf(costo_movimiento(ir, ir->mapaInterno[sucs[h].nodo.x][sucs[h].nodo.y], 
            ir->mapaInterno[u.nodo.x][u.nodo.y]),
            ir->mapaInterno[sucs[h].nodo.x][sucs[h].nodo.y].g);

            if (v < minVal) {
                minVal = v;
            }
        }
        free(sucs);
        // siguiente nodo a recorrer
        ir->mapaInterno[u.nodo.x][u.nodo.y].rhs = minVal;
        ir->mapaInterno[u.nodo.x][u.nodo.y].g = u.g;

        fprintf(stderr, "rhs := %d\n", minVal);
        sk.key = obt_key(ir->mapaInterno[u.nodo.x][u.nodo.y], ir->mapaInterno[ir->x][ir->y]);
        fprintf(stderr, "obtuvo la key, (%d,%d)\n", sk.key.id_1, sk.key.id_2);
    } 
    fprintf(stderr, "busca y elimina\n");
    bheap_buscar_eliminar(ir->cp, &sk);
    //fprintf(stderr, "busco y elimino\n");

    // si el nodo no es consistente, agregar al heap
    if (ir->mapaInterno[u.nodo.x][u.nodo.y].rhs != 
    ir->mapaInterno[u.nodo.x][u.nodo.y].g) {
        ir->cp = bheap_insertar(ir->cp, &sk);
    }

}

void ComputeShortestPath(InfoRobot* ir) {
    //fprintf(stderr, "ComputeShortestPath()\n");
    int cond1 = 1, cond2 = 1;

    //ir->mapaInterno[ir->x][ir->y].nodo.x = ir->x;
    //ir->mapaInterno[ir->x][ir->y].nodo.y = ir->y;

    // while (U.TopKey() < CalculateKey(s_start) OR rhs(s_start) != g(s_start))
    while ((cond1 = ((!bheap_es_vacio(ir->cp) && (comp_keys(((EstadoConClave*)bheap_minimo(ir->cp))->key, 
    obt_key(ir->mapaInterno[ir->x][ir->y], ir->mapaInterno[ir->x][ir->y]))) < 0)))
    || (cond2 = ((ir->mapaInterno[ir->x][ir->y].rhs) != (ir->mapaInterno[ir->x][ir->y].g))) 
  ) {   

        // MAPA:
        fprintf(stderr, "MAPA\n");
        //impr_mapa(ir);
        fprintf(stderr, "rastro hasta ahora: %s\n", ir->rastro);
        ir->mapaInterno[ir->x][ir->y].nodo.x = ir->x;
        ir->mapaInterno[ir->x][ir->y].nodo.y = ir->y;
        
        // popear el minimo elemento de la cola
        State u = *(State*)bheap_minimo(ir->cp);

        ir->cp = bheap_eliminar_minimo(ir->cp);
    

        if (ir->mapaInterno[u.nodo.x][u.nodo.y].g > ir->mapaInterno[u.nodo.x][u.nodo.y].rhs) {
        //fprintf(stderr, "Es sobreconsistente (g > rhs). Poner g = rhs\n");
            ir->mapaInterno[u.nodo.x][u.nodo.y].g = ir->mapaInterno[u.nodo.x][u.nodo.y].rhs;
            int cantPred = 0;
            State* pred = obt_ady(ir, u, &cantPred);
            
            for (int h = 0; h < cantPred; h++) {
                UpdateVertex(pred[h], ir);
            }
            free(pred);
        } 
        else {
            fprintf(stderr, "No es sobreconsistente (g <= rhs), poner g = inf\n");
            u.g = infty();
           
            UpdateVertex(u, ir);
            int cantPred = 0;
            State* pred = obt_ady(ir, u, &cantPred);
            for (int h = 0; h < cantPred; h++) {
                UpdateVertex(pred[h], ir);
            }
            free(pred);
    }
}
//fprintf(stderr, "termina de computar\n");
}

void actualizar_segun_direccion(InfoRobot* ir, int dist, int dx, int dy, int* o, int multiplesOpciones) {
    
    // actualizar casillas validas descubiertas
    for (int h = 1; h < dist; h++) {
        if (ir->mapaInterno[ir->x + (h * dx)][ir->y + (h * dy)].tipoCasilla == DESCONOCIDO)
            ir->mapaInterno[ir->x + (h * dx)][ir->y + (h * dy)].tipoCasilla = VALIDO;
    }
    // detecto obstaculo, actualizar
    if (dist <= ir->distSensorConocida) {
        // actualizar solamente si la celda no excede los limites del mapa
        if ( (ir->y + (dist * dy)) >= 0 && (ir->y + (dist * dy)) < (ir->M) &&
        (ir->x + (dist * dx)) >= 0 && (ir->x + (dist * dx)) < (ir->N ) ) {
            
            //fprintf(stderr,"obstaculo detectado\n");
            int indXObstaculo = ir->x + (dist * dx);
            int indYObstaculo = ir->y + (dist * dy);
            ir->mapaInterno[indXObstaculo][indYObstaculo].tipoCasilla = OBSTACULO;
            (*o)++;
       
            ir->mapaInterno[indXObstaculo][indYObstaculo].nodo.x = indXObstaculo;
            ir->mapaInterno[indXObstaculo][indYObstaculo].nodo.y = indYObstaculo;

            //fprintf(stderr, "casilla: %d,%d\n", ir->mapaInterno[indXObstaculo][indYObstaculo].nodo.x, ir->mapaInterno[indXObstaculo][indYObstaculo].nodo.y);
            
            int contAdyacentes = 0;
            State* ady = obt_ady(ir, ir->mapaInterno[indXObstaculo][indYObstaculo], &contAdyacentes);
            for (int h = 0; h < contAdyacentes; h++) {
                UpdateVertex(ady[h], ir);
            }
            free(ady);
            UpdateVertex(ir->mapaInterno[indXObstaculo][indYObstaculo], ir);
            //ir->mapaInterno[ir->x][ir->y].nodo.x = ir->x;
            //ir->mapaInterno[ir->x][ir->y].nodo.y = ir->y;

            //fprintf(stderr, "s_start key1: [%d, %d]\n",
            //obt_key(ir->mapaInterno[ir->x][ir->y], ir->mapaInterno[ir->x][ir->y]).id_1,
            //obt_key(ir->mapaInterno[ir->x][ir->y], ir->mapaInterno[ir->x][ir->y]).id_2);
          
            ComputeShortestPath(ir);
         
            if (*o == 2 && multiplesOpciones) {
                (ir->mapaInterno[ir->x + dx][ir->y + dy].g) ++;
            }
        }
    }
}

void actualizar_mapa_interno(InfoRobot* ir, int* d, int hayMultiplesOp) {
    int cantObstaculos = 0;
    actualizar_segun_direccion(ir, d[0], -1, 0, &cantObstaculos, hayMultiplesOp); // arriba
    actualizar_segun_direccion(ir, d[1], 1, 0, &cantObstaculos, hayMultiplesOp); // abajo
    actualizar_segun_direccion(ir, d[2], 0, -1, &cantObstaculos, hayMultiplesOp); // izq
    actualizar_segun_direccion(ir, d[3], 0, 1, &cantObstaculos, hayMultiplesOp); // der
}

int siguiente_movimiento(InfoRobot* ir, State* posibles) {
    int sucCount = 0, posiblesMovimientos = 0;
    State* sucs = obt_ady(ir, ir->mapaInterno[ir->x][ir->y], &sucCount);

    int minVal = infty();

    for (int h = 0; h < sucCount; h++) {
        int v = suma_inf(costo_movimiento(ir, ir->mapaInterno[ir->x][ir->y],
        ir->mapaInterno[sucs[h].nodo.x][sucs[h].nodo.y]),
        ir->mapaInterno[sucs[h].nodo.x][sucs[h].nodo.y].g);

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
    fprintf(stderr, "Proximo mov -> (%d, %d)\n", posibles[0].nodo.x, posibles[0].nodo.y);
    if (posiblesMovimientos == 2) fprintf(stderr, "o... (%d, %d)\n", posibles[1].nodo.x, posibles[1].nodo.y);
    else fprintf(stderr, "(solo ese)\n");
    return posiblesMovimientos;
}

// se asume que (ir->x, ir->y) != (sig.x, sig.y)
int mover_robot(InfoRobot* ir, Nodo sig, int pasos) {
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