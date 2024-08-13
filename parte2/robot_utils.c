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

float mult (float a, float b) {
    int inf = infty();
    if (a != inf && b != inf) return a*b;
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


// node.x no sirve casi nunca!
void impr_mapa(InfoRobot* ir) {
    fprintf(stderr, "(estado, g, rhs)\n");
    for (int i = 0; i < ir->N; i++) {
        for (int j = 0; j < ir->M; j++) {
            fprintf(stderr,"(%d, %d, %d)",
            ir->mapaInterno[i][j].tipoCasilla, 
            ir->mapaInterno[i][j].g,
            ir->mapaInterno[i][j].rhs);
            if (i == ir->x && j == ir->y) 
            fprintf(stderr, "*\t");
            else fprintf(stderr, "\t");
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
    ir->x = ir->i1; ir->y = ir->j1;
    ir->mapaInterno[ir->i1][ir->j1].tipoCasilla = VISITADO;
    ir->mapaInterno[ir->i2][ir->j2].rhs = 0;
    
    ir->mapaInterno[ir->i2][ir->j2].node.x = ir->i2;
    ir->mapaInterno[ir->i2][ir->j2].node.y = ir->j2;
    ir->mapaInterno[ir->i1][ir->j1].node.x = ir->i1;
    ir->mapaInterno[ir->i1][ir->j1].node.y = ir->j1;

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
        adyacentes[*adyCount].g = ir->mapaInterno[curr.node.x][curr.node.y + 1].g;
        adyacentes[*adyCount].rhs = ir->mapaInterno[curr.node.x][curr.node.y + 1].rhs;
        (*adyCount)++;
    }
    if (curr.node.y > 0) {
        adyacentes[*adyCount].node.x = curr.node.x;
        adyacentes[*adyCount].node.y = curr.node.y - 1;
        adyacentes[*adyCount].g = ir->mapaInterno[curr.node.x][curr.node.y - 1].g;
        adyacentes[*adyCount].rhs = ir->mapaInterno[curr.node.x][curr.node.y - 1].rhs;
        (*adyCount)++;
    } 
    if (curr.node.x < ir-> N - 1) {
        adyacentes[*adyCount].node.x = curr.node.x + 1;
        adyacentes[*adyCount].node.y = curr.node.y;
        adyacentes[*adyCount].g = ir->mapaInterno[curr.node.x +1][curr.node.y ].g;
        adyacentes[*adyCount].rhs = ir->mapaInterno[curr.node.x +1][curr.node.y ].rhs;
        (*adyCount)++;
    }
    if (curr.node.x > 0) {
        adyacentes[*adyCount].node.x = curr.node.x - 1;
        adyacentes[*adyCount].node.y = curr.node.y;
        adyacentes[*adyCount].g = ir->mapaInterno[curr.node.x -1][curr.node.y].g;
        adyacentes[*adyCount].rhs = ir->mapaInterno[curr.node.x -1][curr.node.y].rhs;
        (*adyCount)++;
    }
    return adyacentes;
}

//c(u, s')?
void UpdateVertex(State u, InfoRobot* ir) {
    fprintf(stderr, "updatevertex: %d,%d\n", u.node.x, u.node.y);
    //fprintf(stderr, "robot aca: %d,%d\n", ir->x, ir->y);

    ir->mapaInterno[ir->x][ir->y].node.x = ir->x;
    ir->mapaInterno[ir->x][ir->y].node.y = ir->y;
    ir->mapaInterno[u.node.x][u.node.y].g = u.g;
    
    Key clave = calcular_key(u, ir->mapaInterno[ir->x][ir->y]);
    State* est = crea_estado(u.node, clave);
    est->g = u.g; est->rhs = u.rhs;
    //printf("Current: (%d, %d)\n", est->node.x, est->node.y);
    // (u_x,u_y) != (i2,j2)
    if (u.node.x != ir->i2 || u.node.y != ir->j2) {

        int sucCount = 0;
        State* sucs = obt_ady(ir, u, &sucCount);

        int minVal = infty();
        
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
        ir->mapaInterno[u.node.x][u.node.y].g = u.g;
        ir->mapaInterno[u.node.x][u.node.y].node.x = u.node.x; 
        ir->mapaInterno[u.node.x][u.node.y].node.y = u.node.y;
        fprintf(stderr, "rhs := %d\n", minVal);
        est->k = calcular_key(ir->mapaInterno[u.node.x][u.node.y], ir->mapaInterno[ir->x][ir->y]);
        free(sucs);
    } 

    //printf("Se ejecuta buscar y elim (%d, %d)\n", u.node.x, u.node.y);
    int e = bheap_buscar_eliminar(ir->cp, &u);
    fprintf(stderr, "%s\n", e>0? "Se encontro y elimino" : "No encontrado");

    // si el nodo no es consistente, agregar al heap
    if (ir->mapaInterno[u.node.x][u.node.y].rhs != 
    ir->mapaInterno[u.node.x][u.node.y].g) {
        fprintf(stderr, "agrega %d,%d; con clave [%d,%d]\n", est->node.x,est->node.y,
        est->k.key1, est->k.key2);
        ir->cp = bheap_insertar(ir->cp, est);
    }

}

int comp_key(Key kA, Key kB) {
    if (kA.key1 < kB.key1) return -1;
    if (kA.key1 > kB.key1) return 1;
    return (kA.key2 < kB.key2) ? -1 : (kA.key2 > kB.key2) ? 1 : 0;
}

void ComputeShortestPath(InfoRobot* ir) {
    fprintf(stderr, "ComputeShortestPath()\n");
    int cond1, cond2;

// Pred(curr): conj casillas desde las cuales puedo llegar a curr
// Suc(curr): conj casillas a las que puedo llegar desde curr
// 4 puntos cardinales, impedimentos? obstaculo en alguna direccion.

    //printf("Llave tope: (%.1lf, %.1lf)\n", llaveTope.key1, llaveTope.key2)

// while (U.TopKey() < CalculateKey(s_start) OR rhs(s_start) != g(s_start))
    while ((cond1 = (ir->mapaInterno[ir->x][ir->y].rhs) != (ir->mapaInterno[ir->x][ir->y].g))
    ||(cond2 = ((!bheap_es_vacio(ir->cp) && (comp_key(((State*)bheap_minimo(ir->cp))->k, 
    calcular_key(ir->mapaInterno[ir->x][ir->y], ir->mapaInterno[ir->x][ir->y]))) < 0))) 
  ) {
        fprintf(stderr, "entra al bucle, cond1: %d, cond2: %d\n", cond1, cond2);

        //fprintf(stderr, "----\nMAPA:\n----\n"); impr_mapa(ir);

        fprintf(stderr, "----\nBHEAP:\n----\n"); bheap_recorrer(ir->cp, imprime_nodo);
        
        // popear el maximo elemento de la cola
        State u = *(State*)bheap_minimo(ir->cp);
        ir->cp = bheap_eliminar_minimo(ir->cp);

        fprintf(stderr, "minimo: %d,%d\n", u.node.x, u.node.y);
        fprintf(stderr, "clave1: %d\n", calcular_key(u, ir->mapaInterno[ir->x][ir->y]).key1);

        // no descubierto
        if (ir->mapaInterno[u.node.x][u.node.y].g > 
        ir->mapaInterno[u.node.x][u.node.y].rhs) {
        fprintf(stderr, "Es sobreconsistente (g > rhs). Poner g = rhs\n");
            ir->mapaInterno[u.node.x][u.node.y].g = ir->mapaInterno[u.node.x][u.node.y].rhs;
            int cantPred = 0;
            State* pred = obt_ady(ir, u, &cantPred);
            
            for (int h = 0; h < cantPred; h++) {
                UpdateVertex(pred[h], ir);
            }
        } 
        else {
            fprintf(stderr, "No es sobreconsistente (g <= rhs), poner g = inf\n");
            ir->mapaInterno[u.node.x][u.node.y].g = infty();
           
            UpdateVertex(u, ir);
            int cantPred = 0;
            State* pred = obt_ady(ir, u, &cantPred);
            for (int h = 0; h < cantPred; h++) {
                UpdateVertex(pred[h], ir);
            }
            
    }

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
            /*if (ir->mapaInterno[indXObstaculo][indYObstaculo].g == infty()) {
                return;
            }*/

            Node n = (Node){indXObstaculo, indYObstaculo};
            ir->mapaInterno[indXObstaculo][indYObstaculo].node.x = indXObstaculo;
            ir->mapaInterno[indXObstaculo][indYObstaculo].node.y = indYObstaculo;
            //ir->mapaInterno[indXObstaculo][indYObstaculo].rhs = infty();

            fprintf(stderr, "casilla: %d,%d\n", ir->mapaInterno[indXObstaculo][indYObstaculo].node.x, ir->mapaInterno[indXObstaculo][indYObstaculo].node.y);
            
            int contAdyacentes = 0;
            State* ady = obt_ady(ir, ir->mapaInterno[indXObstaculo][indYObstaculo], &contAdyacentes);
            for (int h = 0; h < contAdyacentes; h++) {
                UpdateVertex(ady[h], ir);
            }
               
            ir->mapaInterno[ir->x][ir->y].node.x = ir->x;
            ir->mapaInterno[ir->x][ir->y].node.y = ir->y;

            fprintf(stderr, "s_start key1: [%d, %d]\n",
            calcular_key(ir->mapaInterno[ir->x][ir->y], ir->mapaInterno[ir->x][ir->y]).key1,
            calcular_key(ir->mapaInterno[ir->x][ir->y], ir->mapaInterno[ir->x][ir->y]).key2);
                
            State mincp = *(State*)bheap_minimo(ir->cp);
            fprintf(stderr, "el minimo de la cp: %d,%d con g %d y rhs %d\n", mincp.node.x,
            mincp.node.y, mincp.g, mincp.rhs);

            fprintf(stderr, "cp min key [%d, %d]\n",
            calcular_key(mincp, ir->mapaInterno[ir->x][ir->y]).key1,
            calcular_key(mincp, ir->mapaInterno[ir->x][ir->y]).key2);
                
            fprintf(stderr, "bheap: \n"); bheap_recorrer(ir->cp, imprime_nodo);

            fprintf(stderr, "actual: %d,%d\n", ir->mapaInterno[ir->x][ir->y].node.x,
            ir->mapaInterno[ir->x][ir->y].node.y);
            //fprintf(stderr, "heuristica: %d\n", heuristic(mincp.node, ir->mapaInterno[]));

            ComputeShortestPath(ir);
                
      
            //fprintf(stderr, "el robot scanea desde %d,%d\n",ir->x, ir->y);
            //fprintf(stderr, "dx: %d dy: %d\n", dx, dy);
            //fprintf(stderr, "cambia %d, %d\n", ir->x + dx, ir->y + dy);
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

int siguiente_movimiento(InfoRobot* ir, State* posibles) {
    int sucCount = 0, posiblesMovimientos = 0;
    State curr; curr.node.x = ir->x ; curr.node.y = ir->y;
    State* sucs = obt_ady(ir, curr, &sucCount);

    int minVal = infty();
    int hayConocidos = 0;

    for (int h = 0; h < sucCount; h++) {
        // no funca, pero la idea va por aca
        if (ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].tipoCasilla == VALIDO &&
        ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].tipoCasilla != VISITADO) {
            fprintf(stderr, "hay conocidos: %d, %d\n", sucs[h].node.x, sucs[h].node.y);
            hayConocidos = 1;
        }
    }

    if (hayConocidos) {
        //fprintf(stderr, "guarda! hay conocidos\n");
        for (int h = 0; h < sucCount; h++) {
            if (ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].tipoCasilla == DESCONOCIDO) {
                ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].g =
                mult(ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].g, 1);
            }
        }
    } 

    for (int h = 0; h < sucCount; h++) {
        int v = sum(cost(ir, ir->mapaInterno[curr.node.x][curr.node.y],
        ir->mapaInterno[sucs[h].node.x][sucs[h].node.y]),
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