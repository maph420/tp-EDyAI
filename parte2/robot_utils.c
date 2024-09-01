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

/* En un mapa de n x m, la distancia maxima de manhattan
entre dos puntos es n+m-2, en efecto:
|0 - (n-1)| + |0 - (m-1)| = n-1+m-1 = n+m-2
De modo que, basta asignar un "infinito" en nuestro universo
como 2(n+m), de modo que ningun valor puede ser mayor que el.
Multiplicando por 2 porque una key puede tener 2k, con k
la mayor dist manhattan entre dos puntos
*/
// chequear
int infty(int n, int m) {
    return (n + m) * 2;
}

// suma en base al infinito designado
int suma_inf (int a, int b, int n, int m) {
    int inf = infty(n, m);
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
int dist_manhattan(Pos a, Pos b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Implementación de la función key
Key obt_key(Estado s, InfoRobot* ir) {
    int min_g_rhs =  (ir->mapaInterno[s.pos.x][s.pos.y].g < ir->mapaInterno[s.pos.x][s.pos.y].rhs) ? 
    ir->mapaInterno[s.pos.x][s.pos.y].g : ir->mapaInterno[s.pos.x][s.pos.y].rhs;
    return (Key){suma_inf(min_g_rhs, dist_manhattan(ir->mapaInterno[ir->x][ir->y].pos, s.pos),ir->N,ir->M), min_g_rhs};
} 

int g_val(InfoRobot* ir, Pos n) {
    return dist_manhattan(n, ir->mapaInterno[ir->i2][ir->j2].pos);
}

void imprime_nodo(void* refNodo) {
    EstadoConClave sk = *(EstadoConClave*)refNodo;
    fprintf(stderr, "(%d, %d); key: (%d, %d)\n", sk.est.pos.x, sk.est.pos.y, sk.key.id_1, sk.key.id_2);
}

int comp_keys(Key kA, Key kB) {
    return (kA.id_1 < kB.id_1) ? -1 : 
    (kA.id_1 > kB.id_1) ? 1 :
    (kA.id_2 < kB.id_2) ? -1 : 
    (kA.id_2 > kB.id_2) ? 1 : 
    0;
}

int compara_estado_con_clave(void* sk1, void* sk2) {
    if ( ((*(EstadoConClave*)sk1).est.pos.x == (*(EstadoConClave*)sk2).est.pos.x) &&
    ((*(EstadoConClave*)sk1).est.pos.y == (*(EstadoConClave*)sk2).est.pos.y)) {
        return 2;
    }
    Key kA = (Key){(*(EstadoConClave*)sk1).key.id_1, (*(EstadoConClave*)sk1).key.id_2};
    Key kB = (Key){(*(EstadoConClave*)sk2).key.id_1, (*(EstadoConClave*)sk2).key.id_2};
    return comp_keys(kA, kB);
}

// para el heap, podria usar simplemente nodos, agregando al tipo de nodo Key
void* copia_est_con_clave(void* s) {
    EstadoConClave* sk = malloc(sizeof(EstadoConClave));
    sk->est.pos = (Pos){((EstadoConClave*)s)->est.pos.x, ((EstadoConClave*)s)->est.pos.y};
    sk->key = (Key){((EstadoConClave*)s)->key.id_1,((EstadoConClave*)s)->key.id_2};
    sk->est.g = ((EstadoConClave*)s)->est.g;
    sk->est.rhs = ((EstadoConClave*)s)->est.rhs;
    sk->est.tipoCelda = ((EstadoConClave*)s)->est.tipoCelda;
    return sk;
}

void destruir_est_con_clave(void* s) {
    free((EstadoConClave*)s);
}

// El costo entre dos estados es infinito si alguno es un obstaculo 
// (imposible moverse entre ellos). En otro caso, es 1.
int costo_movimiento(InfoRobot* ir, Estado s1, Estado s2) {
    return (s1.tipoCelda == OBSTACULO || s2.tipoCelda == OBSTACULO) ? 
    infty(ir->N, ir->M) : 1;
}

// sacar
void impr_key(Key k) {
    fprintf(stderr,"(%d,%d)\n", k.id_1, k.id_2);
}
// sacar
void impr_mapa(InfoRobot* ir) {
    fprintf(stderr, "(estado, g, rhs)\n");
    for (int i = 0; i < ir->N; i++) {
        for (int j = 0; j < ir->M; j++) {
            fprintf(stderr,"(%d, %d, %d)",
            ir->mapaInterno[i][j].tipoCelda, 
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
    // es la minima que podria tener, ya que 0 no se admite
    ir->distSensorConocida = 1;
    ir->rastro = malloc(sizeof(char) * 150);
    ir->cp = bheap_crear(ir->N * ir->M, compara_estado_con_clave, destruir_est_con_clave, copia_est_con_clave);
    ir->mapaInterno = malloc(sizeof(Estado*) * ir->N);
    // ubicar al robot en la pos inicial dada
    ir->x = ir->i1; ir->y = ir->j1;

    for (int i = 0; i < ir->N; i++) {
        ir->mapaInterno[i] = malloc(sizeof(Estado) * ir->M);
        for (int j = 0; j < ir->M; j++) {
            ir->mapaInterno[i][j].rhs = infty(ir->N, ir->M); 
            ir->mapaInterno[i][j].g = infty(ir->N, ir->M);
            ir->mapaInterno[i][j].tipoCelda = DESCONOCIDO;
            ir->mapaInterno[i][j].pos = (Pos){i,j};
        }
    }

    //printf("jijodebu\n");
    ir->mapaInterno[ir->i1][ir->j1].tipoCelda = VISITADO;
    ir->mapaInterno[ir->i2][ir->j2].rhs = 0;

    Key claveMeta = obt_key(ir->mapaInterno[ir->i2][ir->j2], ir);
    EstadoConClave estadoMeta = {ir->mapaInterno[ir->i2][ir->j2], 
    claveMeta};
    
    fprintf(stderr,"key de la meta:"); impr_key(claveMeta);
    ir->cp = bheap_insertar(ir->cp, &estadoMeta);
}

int asigna_adyacencias(Estado* S, Estado curr, InfoRobot* ir, int dx, int dy, int c) {
    S[c].pos = (Pos){curr.pos.x + dx, curr.pos.y + dy};
    S[c].g = ir->mapaInterno[curr.pos.x + dx][curr.pos.y + dy].g;
    S[c].rhs = ir->mapaInterno[curr.pos.x + dx][curr.pos.y + dy].rhs;
    S[c].tipoCelda = ir->mapaInterno[curr.pos.x + dx][curr.pos.y + dy].tipoCelda;
    return ++c;
}

// todo: cambiar el argumento State por Nodo (con el Nodo alcanza)
Estado* obt_ady(InfoRobot* ir, Estado curr, int* adyCount) {
    Estado* adyacentes = malloc(sizeof(Estado) * 4);
    // malloc en cada vuelta
    if (curr.pos.y < ir-> M - 1)
        *adyCount = asigna_adyacencias(adyacentes, curr, ir, 0, 1, *adyCount);
    if (curr.pos.y > 0) 
        *adyCount = asigna_adyacencias(adyacentes, curr, ir, 0, -1, *adyCount);
    if (curr.pos.x < ir-> N - 1) 
        *adyCount = asigna_adyacencias(adyacentes, curr, ir, 1, 0, *adyCount);
    if (curr.pos.x > 0) 
        *adyCount = asigna_adyacencias(adyacentes, curr, ir, -1, 0, *adyCount);
    return adyacentes;
}

void UpdateVertex(Estado u, InfoRobot* ir) {
    fprintf(stderr, "UpdateVertex(): %d,%d\n", u.pos.x, u.pos.y);

    EstadoConClave sk = {u, obt_key(u, ir)};

    // (u_x,u_y) != (i2,j2)
    if (u.pos.x != ir->i2 || u.pos.y != ir->j2) {

        int sucCount = 0;
        Estado* sucs = obt_ady(ir, u, &sucCount);

        int minVal = infty(ir->N, ir->M);
        for (int h = 0; h < sucCount; h++) {

            int v = suma_inf(costo_movimiento(ir, sucs[h], 
            u), sucs[h].g, ir->N, ir->M);

            if (v < minVal) {
                minVal = v;
            }
        }
        free(sucs);

        // actualizamos el est pasado como argumento como a su ref en el mapa
        u.rhs = minVal; 
        ir->mapaInterno[u.pos.x][u.pos.y].rhs = minVal;

        fprintf(stderr, "rhs := %d\n", minVal);
        sk.key = obt_key(u, ir);
        fprintf(stderr, "obtuvo la key, (%d,%d)\n", sk.key.id_1, sk.key.id_2);
    } 
    
    fprintf(stderr, "busca y elimina\n");
    bheap_buscar_eliminar(ir->cp, &sk);

    fprintf(stderr,"g:%d vs rhs:%d\n", u.g,u.rhs);
    fprintf(stderr,"mapa int g:%d vs rhs:%d\n", ir->mapaInterno[u.pos.x][u.pos.y].g,
    ir->mapaInterno[u.pos.x][u.pos.y].rhs);

    // si el nodo no es consistente, agregar al heap
    if (u.rhs != u.g) {
        fprintf(stderr, "inserta (%d,%d) con clave:", sk.est.pos.x,sk.est.pos.y);
        impr_key(sk.key);
        ir->cp = bheap_insertar(ir->cp, &sk);
    }
}

void ComputeShortestPath(InfoRobot* ir) {
    fprintf(stderr, "ComputeShortestPath()\n");
    int cond1 = 1, cond2 = 1;
    EstadoConClave uc;
    Estado u;
    // while (U.TopKey() < CalculateKey(s_start) OR rhs(s_start) != g(s_start))
    while ((cond1 = ((!bheap_es_vacio(ir->cp) && (comp_keys(((EstadoConClave*)bheap_minimo(ir->cp))->key, 
    obt_key(ir->mapaInterno[ir->x][ir->y], ir))) < 0)))
    || (cond2 = ((ir->mapaInterno[ir->x][ir->y].rhs) != (ir->mapaInterno[ir->x][ir->y].g))) 
  ) {   

        fprintf(stderr, "rastro hasta ahora: %s\n", ir->rastro);
        
        // popear el minimo elemento de la cola
        uc = (*(EstadoConClave*)bheap_minimo(ir->cp));
        ir->cp = bheap_eliminar_minimo(ir->cp);
        u = uc.est;

        fprintf(stderr, "popea:"); imprime_nodo(&u);

        // u.g > u.rhs no anda
        fprintf(stderr, "(u.g, u.rhs): %d,%d\n mapa inter: (u.g, u.rhs): %d,%d\n",
        u.g, u.rhs, ir->mapaInterno[u.pos.x][u.pos.y].g,
        ir->mapaInterno[u.pos.x][u.pos.y].rhs);

        if (u.g > ir->mapaInterno[u.pos.x][u.pos.y].rhs) {
            fprintf(stderr, "Es sobreconsistente (g > rhs). Poner g = rhs\n");
            ir->mapaInterno[u.pos.x][u.pos.y].g = ir->mapaInterno[u.pos.x][u.pos.y].rhs;
            u.g = u.rhs; //
            int adyCount = 0;
            Estado* ady = obt_ady(ir, u, &adyCount);
            
            for (int h = 0; h < adyCount; h++) {
                UpdateVertex(ady[h], ir);
            }
            free(ady);
        } 
        else {
            fprintf(stderr, "No es sobreconsistente (g <= rhs), poner g = inf\n");
            //u.g = infty(ir->N, ir->M);
            ir->mapaInterno[u.pos.x][u.pos.y].g = infty(ir->N, ir->M);
            
            UpdateVertex(ir->mapaInterno[u.pos.x][u.pos.y], ir);
            int adyCount = 0;
            Estado* ady = obt_ady(ir, u, &adyCount);
            for (int h = 0; h < adyCount; h++) {
                UpdateVertex(ady[h], ir);
            }
            free(ady);
    }
}
fprintf(stderr, "termina de computar\n");

        fprintf(stderr, "MAPA\n");
        //impr_mapa(ir);
}

void actualizar_segun_direccion(InfoRobot* ir, int dist, int dx, int dy) {
    
    // actualizar casillas validas descubiertas
    for (int h = 1; h < dist; h++) {
        if (ir->mapaInterno[ir->x + (h * dx)][ir->y + (h * dy)].tipoCelda == DESCONOCIDO)
            ir->mapaInterno[ir->x + (h * dx)][ir->y + (h * dy)].tipoCelda = VALIDO;
    }
    // detecto obstaculo, actualizar
    if (dist <= ir->distSensorConocida) {
        // actualizar solamente si la celda no excede los limites del mapa
        if ( (ir->y + (dist * dy)) >= 0 && (ir->y + (dist * dy)) < (ir->M) &&
        (ir->x + (dist * dx)) >= 0 && (ir->x + (dist * dx)) < (ir->N ) ) {
            
            //fprintf(stderr,"obstaculo detectado\n");
            int indXObstaculo = ir->x + (dist * dx);
            int indYObstaculo = ir->y + (dist * dy);
            ir->mapaInterno[indXObstaculo][indYObstaculo].tipoCelda = OBSTACULO;

            int contAdyacentes = 0;

            Estado* ady = obt_ady(ir, ir->mapaInterno[indXObstaculo][indYObstaculo], &contAdyacentes);
            for (int h = 0; h < contAdyacentes; h++) {
                UpdateVertex(ady[h], ir);
            }
            free(ady);
            UpdateVertex(ir->mapaInterno[indXObstaculo][indYObstaculo], ir);

            fprintf(stderr, "s_start key1: [%d, %d]\n",
            obt_key(ir->mapaInterno[ir->x][ir->y], ir).id_1,
            obt_key(ir->mapaInterno[ir->x][ir->y], ir).id_2);

            // recalcular ruta optima
            ComputeShortestPath(ir);
        }
    }
}

void actualizar_mapa_interno(InfoRobot* ir, int* d) {
    actualizar_segun_direccion(ir, d[0], -1, 0); // arriba
    actualizar_segun_direccion(ir, d[1], 1, 0); // abajo
    actualizar_segun_direccion(ir, d[2], 0, -1); // izq
    actualizar_segun_direccion(ir, d[3], 0, 1); // der
}

int siguiente_movimiento(InfoRobot* ir, Estado* posibles) {
    int adyCount = 0, posiblesMov = 0;
    Estado* ady = obt_ady(ir, ir->mapaInterno[ir->x][ir->y], &adyCount);

    int minVal = infty(ir->N, ir->M);

    for (int h = 0; h < adyCount; h++) {
        fprintf(stderr,"costo mov: (%d,%d)->(%d,%d): %d\n",
        ir->x,ir->y, ady[h].pos.x, ady[h].pos.y,
        costo_movimiento(ir, ir->mapaInterno[ir->x][ir->y],
        ady[h]));

        int v = suma_inf(costo_movimiento(ir, ir->mapaInterno[ir->x][ir->y],
        ady[h]), ady[h].g, ir->N, ir->M);
 
        if (v < minVal) {
            minVal = v;
            posibles[0] = ady[h];
            posiblesMov = 1;
        } else if (v == minVal) {
            posibles[1] = ady[h];
            posiblesMov = 2;
        }
    }
    free(ady);
    fprintf(stderr, "Proximo mov -> (%d, %d)\n", posibles[0].pos.x, posibles[0].pos.y);
    if (posiblesMov == 2) fprintf(stderr, "o... (%d, %d)\n", posibles[1].pos.x, posibles[1].pos.y);
    else fprintf(stderr, "(solo ese)\n");
    return posiblesMov;
}

// se asume que (ir->x, ir->y) != (sig.x, sig.y)
int mover_robot(InfoRobot* ir, Pos sig, int pasos) {
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