#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "robot_utils.h"

// sacar
void imprime_nodo(void* refNodo) {
    EstadoConClave sk = *(EstadoConClave*)refNodo;
    fprintf(stderr, "(%d, %d); key: (%d, %d)\n", sk.est.coord.x, sk.est.coord.y, sk.key.id_1, sk.key.id_2);
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

// sacar
void impr_key(Key k) {
    fprintf(stderr,"(%d,%d)\n", k.id_1, k.id_2);
}

// retorna 0 o 1 de manera aleatoria
int aleatorio() {
    srand(time(NULL)) ;
    return (rand() % 2);
}

// arreglar
int infty(InfoRobot* ir) {
    return (ir->N * ir->M * 100);
}

int suma_inf (int a, int b, InfoRobot* ir) {
    int inf = infty(ir);
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
int dist_manhattan(Coord a, Coord b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Implementación de la función key
Key obt_key(Estado s, InfoRobot* ir) {
    int min_g_rhs =  (ir->mapaInterno[s.coord.x][s.coord.y].g < ir->mapaInterno[s.coord.x][s.coord.y].rhs) ? 
    ir->mapaInterno[s.coord.x][s.coord.y].g : ir->mapaInterno[s.coord.x][s.coord.y].rhs;
    
    return (Key){suma_inf(min_g_rhs, suma_inf(dist_manhattan(ir->mapaInterno[ir->x][ir->y].coord, s.coord), ir->km, ir), ir), 
    min_g_rhs};
} 

int g_val(InfoRobot* ir, Coord n) {
    return dist_manhattan(n, ir->mapaInterno[ir->i2][ir->j2].coord);
}

int comp_keys(Key kA, Key kB) {
    return (kA.id_1 < kB.id_1) ? -1 : 
    (kA.id_1 > kB.id_1) ? 1 :
    (kA.id_2 < kB.id_2) ? -1 : 
    (kA.id_2 > kB.id_2) ? 1 : 
    0;
}

int compara_estado_con_clave(void* sk1, void* sk2) {
    if ( ((*(EstadoConClave*)sk1).est.coord.x == (*(EstadoConClave*)sk2).est.coord.x) &&
    ((*(EstadoConClave*)sk1).est.coord.y == (*(EstadoConClave*)sk2).est.coord.y)) {
        return 2;
    }
    Key kA = (Key){(*(EstadoConClave*)sk1).key.id_1, (*(EstadoConClave*)sk1).key.id_2};
    Key kB = (Key){(*(EstadoConClave*)sk2).key.id_1, (*(EstadoConClave*)sk2).key.id_2};
    return comp_keys(kA, kB);
}

// para el heap, podria usar simplemente nodos, agregando al tipo de nodo Key
void* copia_est_con_clave(void* s) {
    EstadoConClave* sk = malloc(sizeof(EstadoConClave));
    sk->est.coord = (Coord){((EstadoConClave*)s)->est.coord.x, ((EstadoConClave*)s)->est.coord.y};
    sk->key = (Key){((EstadoConClave*)s)->key.id_1,((EstadoConClave*)s)->key.id_2};
    sk->est.g = ((EstadoConClave*)s)->est.g;
    sk->est.rhs = ((EstadoConClave*)s)->est.rhs;
    sk->est.tipoCelda = ((EstadoConClave*)s)->est.tipoCelda;
    return sk;
}

void destruir_est_con_clave(void* s) {
    free((EstadoConClave*)s);
}

int costo_movimiento(InfoRobot* ir, Estado s1, Estado s2) {
    return (s1.tipoCelda == OBSTACULO || s2.tipoCelda == OBSTACULO) ? 
    infty(ir) : 1;
}


void InicializaRobot(InfoRobot* ir) {
    ir->km = 0;
    // es la minima que podria tener, ya que 0 no se admite
    ir->distSensorConocida = 1;
    ir->rastro = malloc(sizeof(char) * ir->N * ir->M);
    ir->cp = bheap_crear(ir->N * ir->M, compara_estado_con_clave, destruir_est_con_clave, copia_est_con_clave);
    ir->mapaInterno = malloc(sizeof(Estado*) * ir->N);
    // ubicar al robot en la pos inicial dada
    ir->x = ir->i1; ir->y = ir->j1;

    for (int i = 0; i < ir->N; i++) {
        ir->mapaInterno[i] = malloc(sizeof(Estado) * ir->M);
        for (int j = 0; j < ir->M; j++) {
            ir->mapaInterno[i][j].rhs = infty(ir); 
            ir->mapaInterno[i][j].g = infty(ir);
            ir->mapaInterno[i][j].tipoCelda = DESCONOCIDO;
            ir->mapaInterno[i][j].coord = (Coord){i,j};
        }
    }

    //printf("jijodebu\n");
    ir->mapaInterno[ir->i1][ir->j1].tipoCelda = VISITADO;
    ir->mapaInterno[ir->i2][ir->j2].rhs = 0;

    EstadoConClave estadoMeta = {ir->mapaInterno[ir->i2][ir->j2], 
    obt_key(ir->mapaInterno[ir->i2][ir->j2], ir)};
    
    // hacer bheap insertar void
    bheap_insertar(ir->cp, &estadoMeta);
}

int asigna_adyacencia(Estado* S, Estado curr, InfoRobot* ir, int dx, int dy, int c) {
    S[c].coord = (Coord){curr.coord.x + dx, curr.coord.y + dy};
    S[c].g = ir->mapaInterno[curr.coord.x + dx][curr.coord.y + dy].g;
    S[c].rhs = ir->mapaInterno[curr.coord.x + dx][curr.coord.y + dy].rhs;
    S[c].tipoCelda = ir->mapaInterno[curr.coord.x + dx][curr.coord.y + dy].tipoCelda;
    return ++c;
}

Estado* obt_ady(InfoRobot* ir, Estado curr, int* adyCount) {
    Estado* adyacentes = malloc(sizeof(Estado) * 4);
    // malloc en cada vuelta
    if (curr.coord.y < ir-> M - 1)
        *adyCount = asigna_adyacencia(adyacentes, curr, ir, 0, 1, *adyCount);
    if (curr.coord.y > 0) 
        *adyCount = asigna_adyacencia(adyacentes, curr, ir, 0, -1, *adyCount);
    if (curr.coord.x < ir-> N - 1) 
        *adyCount = asigna_adyacencia(adyacentes, curr, ir, 1, 0, *adyCount);
    if (curr.coord.x > 0) 
        *adyCount = asigna_adyacencia(adyacentes, curr, ir, -1, 0, *adyCount);
    return adyacentes;
}

void UpdateVertex(Estado u, InfoRobot* ir) {

    EstadoConClave sk = {u, obt_key(u, ir)};
    // (u_x,u_y) != (i2,j2)
    if (u.coord.x != ir->i2 || u.coord.y != ir->j2) {

        int sucCount = 0;
        Estado* sucs = obt_ady(ir, u, &sucCount);

        int minVal = infty(ir);
        for (int h = 0; h < sucCount; h++) {

            int v = suma_inf(costo_movimiento(ir, sucs[h], 
            ir->mapaInterno[u.coord.x][u.coord.y]), sucs[h].g, ir);

            if (v < minVal) {
                minVal = v;
            }
        }
        free(sucs);

        u.rhs = minVal; 
        ir->mapaInterno[u.coord.x][u.coord.y].rhs = minVal;

        sk.key.id_1 = obt_key(u, ir).id_1;
        sk.key.id_2 = obt_key(u, ir).id_2;
  
    } 
    
    //fprintf(stderr, "busca y elimina\n");
    bheap_buscar_eliminar(ir->cp, &sk);
  
    //fprintf(stderr,"g:%d vs rhs:%d\n", u.g,u.rhs);
    //fprintf(stderr,"mapa int g:%d vs rhs:%d\n", ir->mapaInterno[u.pos.x][u.pos.y].g,
    //ir->mapaInterno[u.pos.x][u.pos.y].rhs);

    // si el nodo no es consistente, agregar al heap
    if (u.rhs != u.g) {
        bheap_insertar(ir->cp, &sk);
    }
}

void CalcularRutaOptima(InfoRobot* ir) {
    //fprintf(stderr, "ComputeShortestPath()\n");
    int cond1 = 1, cond2 = 1;
    Estado u;
    Key k_old;

    // while (U.TopKey() < CalculateKey(s_start) OR rhs(s_start) != g(s_start))
    while ((cond1 = ((!bheap_es_vacio(ir->cp) && (comp_keys(((EstadoConClave*)bheap_minimo(ir->cp))->key, 
    obt_key(ir->mapaInterno[ir->x][ir->y], ir))) < 0)))
    || (cond2 = ((ir->mapaInterno[ir->x][ir->y].rhs) != (ir->mapaInterno[ir->x][ir->y].g))) 
  ) {   

        u = (*(EstadoConClave*)bheap_minimo(ir->cp)).est;
        k_old = (*(EstadoConClave*)bheap_minimo(ir->cp)).key;
        bheap_eliminar_minimo(ir->cp);

        if (comp_keys(k_old, obt_key(u, ir)) < 0) {
            EstadoConClave tmp;
            tmp.est = u;
            tmp.key = obt_key(u, ir);
            bheap_insertar(ir->cp, &tmp);
        } 

        else if (ir->mapaInterno[u.coord.x][u.coord.y].g > ir->mapaInterno[u.coord.x][u.coord.y].rhs) {
           // fprintf(stderr, "Es sobreconsistente (g > rhs). Poner g = rhs\n");
            ir->mapaInterno[u.coord.x][u.coord.y].g = ir->mapaInterno[u.coord.x][u.coord.y].rhs;
            u.g = u.rhs; //
            int adyCount = 0;
            Estado* ady = obt_ady(ir, u, &adyCount);
            
            for (int h = 0; h < adyCount; h++) {
                UpdateVertex(ady[h], ir);
            }
            free(ady);
        } 
        else {
            ir->mapaInterno[u.coord.x][u.coord.y].g = infty(ir);

            UpdateVertex(ir->mapaInterno[u.coord.x][u.coord.y], ir);
            int adyCount = 0;
            Estado* ady = obt_ady(ir, u, &adyCount);
            for (int h = 0; h < adyCount; h++) {
                UpdateVertex(ady[h], ir);
            }
            free(ady);
    }
}
}

void actualizar_segun_direccion(InfoRobot* ir, int dist, int dx, int dy, Estado last) {
    
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

            ir->km += dist_manhattan(last.coord, ir->mapaInterno[ir->x][ir->y].coord); ;

            last.coord.x = ir->x; last.coord.y = ir->y;

            UpdateVertex(ir->mapaInterno[indXObstaculo][indYObstaculo], ir);

            // recalcular ruta optima
            CalcularRutaOptima(ir);
        }
    }
}

void actualizar_mapa_interno(InfoRobot* ir, int* d, Estado last) {
    actualizar_segun_direccion(ir, d[0], -1, 0, last); // arriba
    actualizar_segun_direccion(ir, d[1], 1, 0, last); // abajo
    actualizar_segun_direccion(ir, d[2], 0, -1, last); // izq
    actualizar_segun_direccion(ir, d[3], 0, 1, last); // der
}

int siguiente_movimiento(InfoRobot* ir, Estado* posibles) {
    int adyCount = 0, posiblesMov = 0;
    Estado* ady = obt_ady(ir, ir->mapaInterno[ir->x][ir->y], &adyCount);

    int minVal = infty(ir);

    for (int h = 0; h < adyCount; h++) {

        int v = suma_inf(costo_movimiento(ir, ir->mapaInterno[ir->x][ir->y],
        ady[h]), ady[h].g, ir);
 
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
    return posiblesMov;
}

int mover_robot(InfoRobot* ir, Coord sig, int pasos) {
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