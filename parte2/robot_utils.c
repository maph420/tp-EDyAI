#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "robot_utils.h"

int aleatorio() {
    srand(time(NULL)) ;
    return (rand() % 2);
}

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

int dist_manhattan(Coord a, Coord b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

Key obt_key(Coord c, InfoRobot* ir) {
    Coord coordRobot = {ir->x, ir->y};
    int min_g_rhs =  (ir->mapaInterno[c.x][c.y].g < ir->mapaInterno[c.x][c.y].rhs) ? 
    ir->mapaInterno[c.x][c.y].g : ir->mapaInterno[c.x][c.y].rhs;

    return (Key){suma_inf(min_g_rhs, suma_inf(dist_manhattan(coordRobot, c), ir->km, ir), ir), 
    min_g_rhs};
} 

int g_val(InfoRobot* ir, Coord n) {
    return dist_manhattan(n, (Coord){ir->i2, ir->j2});
}

int comp_keys(Key kA, Key kB) {
    return (kA.id_1 < kB.id_1) ? -1 : 
    (kA.id_1 > kB.id_1) ? 1 :
    (kA.id_2 < kB.id_2) ? -1 : 
    (kA.id_2 > kB.id_2) ? 1 : 
    0;
}

int compara_coord_con_clave(void* cc1, void* cc2) {
    if ( (((CoordConClave*)cc1)->coord.x == ((CoordConClave*)cc2)->coord.x) &&
    (((CoordConClave*)cc1)->coord.y == ((CoordConClave*)cc2)->coord.y)) {
        return 2;
    }
    Key kA = ((CoordConClave*)cc1)->key;
    Key kB = ((CoordConClave*)cc2)->key;
    return comp_keys(kA, kB);
}

void* copia_coord_con_clave(void* s) {
    CoordConClave* cc = malloc(sizeof(CoordConClave));
    cc->coord = (Coord){((CoordConClave*)s)->coord.x, ((CoordConClave*)s)->coord.y};
    cc->key = (Key){((CoordConClave*)s)->key.id_1,((CoordConClave*)s)->key.id_2};
    return cc;
}

void destruir_coord_con_clave(void* s) {
    free((CoordConClave*)s);
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
    ir->cp = bheap_crear(ir->N * ir->M, compara_coord_con_clave, destruir_coord_con_clave, copia_coord_con_clave);
    ir->mapaInterno = malloc(sizeof(Estado*) * ir->N);
    // ubica al robot en la pos inicial dada
    ir->x = ir->i1; ir->y = ir->j1;

    for (int i = 0; i < ir->N; i++) {
        ir->mapaInterno[i] = malloc(sizeof(Estado) * ir->M);
        for (int j = 0; j < ir->M; j++) {
            ir->mapaInterno[i][j].rhs = infty(ir); 
            ir->mapaInterno[i][j].g = infty(ir);
            ir->mapaInterno[i][j].tipoCelda = DESCONOCIDO;
        }
    }

    ir->mapaInterno[ir->i1][ir->j1].tipoCelda = VISITADO;
    ir->mapaInterno[ir->i2][ir->j2].rhs = 0;

    CoordConClave cc = {(Coord){ir->i2, ir->j2}, obt_key(cc.coord, ir)};

    bheap_insertar(ir->cp, &cc);
}

int asigna_adyacencia(EstadoConCoord* S, Coord curr, InfoRobot* ir, int dx, int dy, int c) {
    S[c].c = (Coord){curr.x + dx, curr.y + dy};
    S[c].est.g = ir->mapaInterno[curr.x + dx][curr.y + dy].g;
    S[c].est.rhs = ir->mapaInterno[curr.x + dx][curr.y + dy].rhs;
    S[c].est.tipoCelda = ir->mapaInterno[curr.x + dx][curr.y + dy].tipoCelda;
    return ++c;
}

EstadoConCoord* obt_ady(InfoRobot* ir, Coord curr, int* adyCount) {

    EstadoConCoord* adyacentes = malloc(sizeof(EstadoConCoord) * 4);

    if (curr.y < ir-> M - 1) {
        *adyCount = asigna_adyacencia(adyacentes, curr, ir, 0, 1, *adyCount);
    }
    if (curr.y > 0) {
        *adyCount = asigna_adyacencia(adyacentes, curr, ir, 0, -1, *adyCount);
    }
        
    if (curr.x < ir-> N - 1) {
         *adyCount = asigna_adyacencia(adyacentes, curr, ir, 1, 0, *adyCount);
    }
    if (curr.x > 0) {
         *adyCount = asigna_adyacencia(adyacentes, curr, ir, -1, 0, *adyCount);
    }
    return adyacentes;
}

void UpdateVertex(Estado u, Coord c, InfoRobot* ir) {

    CoordConClave cc = {(Coord){c.x, c.y}, obt_key(c, ir)};
    // (u_x,u_y) != (i2,j2)
    if (c.x != ir->i2 || c.y != ir->j2) {

        int adyCount = 0;
        EstadoConCoord* ady = obt_ady(ir, c, &adyCount);

        int min = infty(ir);
        for (int h = 0; h < adyCount; h++) {

            int v = suma_inf(costo_movimiento(ir, ady[h].est, 
            ir->mapaInterno[c.x][c.y]), ady[h].est.g, ir);

            if (v < min) {
                min = v;
            }
        }
        free(ady);
        u.rhs = min; 
        ir->mapaInterno[c.x][c.y].rhs = min;
        cc.key = obt_key(c, ir);
    } 

    bheap_buscar_eliminar(ir->cp, &cc);

    // Si el nodo no es consistente, agregar al heap
    if (u.rhs != u.g) {
        bheap_insertar(ir->cp, &cc);
    }
}

void CalcularRutaOptima(InfoRobot* ir) {
    Key k_old;
    Coord coordRobot = {ir->x, ir->y}, u;

    while (((!bheap_es_vacio(ir->cp) && 
    (comp_keys(((CoordConClave*)bheap_minimo(ir->cp))->key, 
    obt_key(coordRobot, ir))) < 0))
    || ((ir->mapaInterno[ir->x][ir->y].rhs) != (ir->mapaInterno[ir->x][ir->y].g)) 
) {   
        // Extraemos el tope de la cola de prioridad
        u = ((CoordConClave*)bheap_minimo(ir->cp))->coord;
        k_old = ((CoordConClave*)bheap_minimo(ir->cp))->key;
        bheap_eliminar_minimo(ir->cp);

        /* Significa que la clave del estado en el momento que fue
        insertado a la cola difiere de la clave que posee ahora 
        (por cambios en la funcion heuristica)  */
        if (comp_keys(k_old, obt_key(u, ir)) < 0) {
            CoordConClave tmp = {u, obt_key(u, ir)};
            bheap_insertar(ir->cp, &tmp);
        } 

        /* El estado es sobreconsistente. Como el valor actualizado rhs es mejor,
        optamos por esta alternativa (actualizando el valor g), y propagando el
        cambio a los estados adyacentes
        */
        else if (ir->mapaInterno[u.x][u.y].g > ir->mapaInterno[u.x][u.y].rhs) {
            // Hacer nodo consistente (se actualizan las referencias al nodo en el mapa interno)
            ir->mapaInterno[u.x][u.y].g = ir->mapaInterno[u.x][u.y].rhs;
            int adyCount = 0;
            EstadoConCoord* ady = obt_ady(ir, u, &adyCount);
            
            for (int h = 0; h < adyCount; h++) {
                UpdateVertex(ady[h].est, ady[h].c, ir);
            }
            free(ady);
        } 
        /*
        El estado es subconsistente, por lo cual no conviene tomar los cambios
        producidos, ya que se espera un valor g mayor al actual (rhs > g). Entonces,
        propagamos los cambios a los nodos adyacentes, y al mismo nodo, de modo que
        se reinserta en la cola de prioridad (de no ser g = rhs) y puede recalcular
        el valor estimado rhs, en base a los costos g estimados actualizados de 
        los vecinos.*/
        else {
            ir->mapaInterno[u.x][u.y].g = infty(ir);

            UpdateVertex(ir->mapaInterno[u.x][u.y], u, ir);
            int adyCount = 0;
            EstadoConCoord* ady = obt_ady(ir, u, &adyCount);
            for (int h = 0; h < adyCount; h++) {
                UpdateVertex(ady[h].est, ady[h].c, ir);
            }
            free(ady);
    }
}

}

void actualizar_segun_direccion(InfoRobot* ir, int dist, int dx, int dy, Coord ant) {
    
    // Actualizar casillas validas descubiertas
    for (int h = 1; h < dist; h++) {
        if (ir->mapaInterno[ir->x + (h * dx)][ir->y + (h * dy)].tipoCelda == DESCONOCIDO)
            ir->mapaInterno[ir->x + (h * dx)][ir->y + (h * dy)].tipoCelda = VALIDO;
    }
    // Detecto obstaculo, actualizar
    if (dist <= ir->distSensorConocida) {

        // Actualizar solamente si la celda no excede los limites del mapa
        if ( (ir->y + (dist * dy)) >= 0 && (ir->y + (dist * dy)) < (ir->M) &&
        (ir->x + (dist * dx)) >= 0 && (ir->x + (dist * dx)) < (ir->N ) ) {
            
            int indXObstaculo = ir->x + (dist * dx);
            int indYObstaculo = ir->y + (dist * dy);
            ir->mapaInterno[indXObstaculo][indYObstaculo].tipoCelda = OBSTACULO;

            ir->km += dist_manhattan(ant, (Coord){ir->x, ir->y});
            ant.x = ir->x; ant.y = ir->y;

            /*
            Para esta implementacion del algoritmo, basta con actualizar el
            nodo que posee el obstaculo.
            */
            UpdateVertex(ir->mapaInterno[indXObstaculo][indYObstaculo], 
            (Coord){indXObstaculo, indYObstaculo}, ir);

            // Recalcular ruta optima
            CalcularRutaOptima(ir);
        }
    }
}

void actualizar_mapa_interno(InfoRobot* ir, int* d, Coord ant) {
    actualizar_segun_direccion(ir, d[0], -1, 0, ant); // arriba
    actualizar_segun_direccion(ir, d[1], 1, 0, ant); // abajo
    actualizar_segun_direccion(ir, d[2], 0, -1, ant); // izq
    actualizar_segun_direccion(ir, d[3], 0, 1, ant); // der
}

int siguiente_movimiento(InfoRobot* ir, EstadoConCoord* posibles) {
    int adyCount = 0, posiblesMov = 0;
    EstadoConCoord* ady = obt_ady(ir, (Coord){ir->x, ir->y}, &adyCount);
    
    int min = infty(ir);
    for (int h = 0; h < adyCount; h++) {

        int v = suma_inf(costo_movimiento(ir, ir->mapaInterno[ir->x][ir->y],
        ady[h].est), ady[h].est.g, ir);
 
        if (v < min) {
            min = v;
            posibles[0] = (EstadoConCoord){ady[h].est, ady[h].c};
            posiblesMov = 1;
        } else if (v == min) {
            posibles[1] = (EstadoConCoord){ady[h].est, ady[h].c};
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