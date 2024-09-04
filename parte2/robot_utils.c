#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "robot_utils.h"

// sacar

void imprime_nodo(void* refNodo) {
    PosConClave sk = *(PosConClave*)refNodo;
    fprintf(stderr, "(%d, %d); key: (%d, %d)\n", sk.coord.x, sk.coord.y, sk.key.id_1, sk.key.id_2);
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
    return (ir->N * ir->M * 10000);
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
    //fprintf(stderr,"KEY:"); impr_key((Key){suma_inf(min_g_rhs, suma_inf(dist_manhattan(coordRobot, c), ir->km, ir), ir), 
    //min_g_rhs});

    return (Key){suma_inf(min_g_rhs, suma_inf(dist_manhattan(coordRobot, c), ir->km, ir), ir), 
    min_g_rhs};
} 

int g_val(InfoRobot* ir, Coord n) {
    Coord coordMeta = {ir->i2, ir->j2};
    return dist_manhattan(n, coordMeta);
}

int comp_keys(Key kA, Key kB) {
    return (kA.id_1 < kB.id_1) ? -1 : 
    (kA.id_1 > kB.id_1) ? 1 :
    (kA.id_2 < kB.id_2) ? -1 : 
    (kA.id_2 > kB.id_2) ? 1 : 
    0;
}

int compara_pos_con_clave(void* sk1, void* sk2) {
    if ( (((PosConClave*)sk1)->coord.x == ((PosConClave*)sk2)->coord.x) &&
    (((PosConClave*)sk1)->coord.y == ((PosConClave*)sk2)->coord.y)) {
        return 2;
    }
    Key kA = ((PosConClave*)sk1)->key;
    Key kB = ((PosConClave*)sk2)->key;
    return comp_keys(kA, kB);
}

// para el heap, podria usar simplemente nodos, agregando al tipo de nodo Key
void* copia_pos_con_clave(void* s) {
    PosConClave* pc = malloc(sizeof(PosConClave));
    Coord c = (Coord){((PosConClave*)s)->coord.x, ((PosConClave*)s)->coord.y};
    Key k = (Key){((PosConClave*)s)->key.id_1,((PosConClave*)s)->key.id_2};
    pc->coord = c;
    pc->key = k;

    return pc;
}

void destruir_pos_con_clave(void* s) {
    free((PosConClave*)s);
}

//
int costo_movimiento(InfoRobot* ir, Estado s1, Estado s2) {
    return (s1.tipoCelda == OBSTACULO || s2.tipoCelda == OBSTACULO) ? 
    infty(ir) : 1;
}

void InicializaRobot(InfoRobot* ir) {
    ir->km = 0;
    // es la minima que podria tener, ya que 0 no se admite
    ir->distSensorConocida = 1;
    ir->rastro = malloc(sizeof(char) * ir->N * ir->M);
    ir->cp = bheap_crear(ir->N * ir->M, compara_pos_con_clave, destruir_pos_con_clave, copia_pos_con_clave);
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

    PosConClave pc;
    pc.coord = (Coord){ir->i2, ir->j2};
    pc.key = obt_key(pc.coord, ir);

  //  fprintf(stderr, "key:"); impr_key(pc.key);

    bheap_insertar(ir->cp, &pc);
    assert(es_bheap(ir->cp) == 1);
}

int asigna_adyacencia(Estado* S, Coord curr, InfoRobot* ir, int dx, int dy, int c) {
    S[c].g = ir->mapaInterno[curr.x + dx][curr.y + dy].g;
    S[c].rhs = ir->mapaInterno[curr.x + dx][curr.y + dy].rhs;
    S[c].tipoCelda = ir->mapaInterno[curr.x + dx][curr.y + dy].tipoCelda;
    return ++c;
}

int asigna_adyacencia2(EstadoConCoord* S, Coord curr, InfoRobot* ir, int dx, int dy, int c) {
    S[c].c.x = curr.x + dx;
    S[c].c.y = curr.y + dy;
    S[c].est.g = ir->mapaInterno[curr.x + dx][curr.y + dy].g;
    S[c].est.rhs = ir->mapaInterno[curr.x + dx][curr.y + dy].rhs;
    S[c].est.tipoCelda = ir->mapaInterno[curr.x + dx][curr.y + dy].tipoCelda;
    return ++c;
}

Estado* obt_ady(InfoRobot* ir, Coord curr, int* adyCount) {

    Estado* adyacentes = malloc(sizeof(Estado) * 4);
    if (curr.y < ir-> M - 1)
        *adyCount = asigna_adyacencia(adyacentes, curr, ir, 0, 1, *adyCount);
    if (curr.y > 0) 
        *adyCount = asigna_adyacencia(adyacentes, curr, ir, 0, -1, *adyCount);
    if (curr.x < ir-> N - 1) 
        *adyCount = asigna_adyacencia(adyacentes, curr, ir, 1, 0, *adyCount);
    if (curr.x > 0) 
        *adyCount = asigna_adyacencia(adyacentes, curr, ir, -1, 0, *adyCount);
    return adyacentes;
}

EstadoConCoord* obt_ady2(InfoRobot* ir, Coord curr, int* adyCount) {
   // fprintf(stderr,"robot es: (%d, %d)\n", ir->x, ir->y);

    EstadoConCoord* adyacentes = malloc(sizeof(EstadoConCoord) * 4);
    if (curr.y < ir-> M - 1) {
        *adyCount = asigna_adyacencia2(adyacentes, curr, ir, 0, 1, *adyCount);
      //   fprintf(stderr, "Estado con pos: (%d,%d) ", adyacentes[*adyCount].c.x, adyacentes[*adyCount].c.y);
         //fprintf(stderr, "g: %d, rhs: %d, estcasilla: %d\n", adyacentes[*adyCount].est.g, 
    //     adyacentes[*adyCount].est.rhs, adyacentes[*adyCount].est.tipoCelda);
    }
    if (curr.y > 0) {
         //fprintf(stderr,"adycount = %d\n", *adyCount);
        *adyCount = asigna_adyacencia2(adyacentes, curr, ir, 0, -1, *adyCount);

        //fprintf(stderr, "Estado con pos: (%d,%d) ", adyacentes[*adyCount].c.x, adyacentes[*adyCount].c.y);
         //fprintf(stderr, "g: %d, rhs: %d, estcasilla: %d\n", adyacentes[*adyCount].est.g, 
         //adyacentes[*adyCount].est.rhs, adyacentes[*adyCount].est.tipoCelda);
    }
        
    if (curr.x < ir-> N - 1) {
         //fprintf(stderr,"adycount = %d\n", *adyCount);
         *adyCount = asigna_adyacencia2(adyacentes, curr, ir, 1, 0, *adyCount);

         //fprintf(stderr, "Estado con pos: (%d,%d) ", adyacentes[*adyCount].c.x, adyacentes[*adyCount].c.y);
         //fprintf(stderr, "g: %d, rhs: %d, estcasilla: %d\n", adyacentes[*adyCount].est.g, 
         //adyacentes[*adyCount].est.rhs, adyacentes[*adyCount].est.tipoCelda);
    }
    if (curr.x > 0) {
         //fprintf(stderr,"adycount = %d\n", *adyCount);
         *adyCount = asigna_adyacencia2(adyacentes, curr, ir, -1, 0, *adyCount);

         //fprintf(stderr, "Estado con pos: (%d,%d) ", adyacentes[*adyCount].c.x, adyacentes[*adyCount].c.y);
        // fprintf(stderr, "g: %d, rhs: %d, estcasilla: %d\n", adyacentes[*adyCount].est.g, 
        // adyacentes[*adyCount].est.rhs, adyacentes[*adyCount].est.tipoCelda);
    }
    return adyacentes;
}


void UpdateVertex(Estado u, Coord c, InfoRobot* ir) {
   // fprintf(stderr, "updatevertex: %d,%d\n", c.x, c.y);
    PosConClave pc;
    Key k = obt_key(c, ir);
    pc.coord = (Coord){c.x, c.y};
    pc.key = k;

    //fprintf(stderr, "NODO:");
    //imprime_nodo(&pc);
    // (u_x,u_y) != (i2,j2)
    if (c.x != ir->i2 || c.y != ir->j2) {

        int adyCount = 0;
        Estado* ady = obt_ady(ir, c, &adyCount);


        int minVal = infty(ir);
        for (int h = 0; h < adyCount; h++) {

            int v = suma_inf(costo_movimiento(ir, ady[h], 
            ir->mapaInterno[c.x][c.y]), ady[h].g, ir);


            if (v < minVal) {
                minVal = v;
            }
        }
        
       //fprintf(stderr, "rhs := %d\n", minVal);
        u.rhs = minVal; 
        ir->mapaInterno[c.x][c.y].rhs = minVal;
        ir->mapaInterno[c.x][c.y].g = u.g; //
        pc.key = obt_key(c, ir);
       // fprintf(stderr, "obtuvo la key: \n"); impr_key(pc.key);
  
    } 

    assert(es_bheap(ir->cp) == 1);

    bheap_buscar_eliminar(ir->cp, &pc);
     //fprintf(stderr, "busco y elimino\n");
    assert(es_bheap(ir->cp) == 1);

    // si el nodo no es consistente, agregar al heap
    if (u.rhs != u.g) {
        bheap_insertar(ir->cp, &pc);
        assert(es_bheap(ir->cp) == 1);
    }
}

void CalcularRutaOptima(InfoRobot* ir) {
    //fprintf(stderr, "ComputeShortestPath()\n");
    Coord u;
    Key k_old;
    Coord coordRobot = {ir->x, ir->y};

    // while (U.TopKey() < CalculateKey(s_start) OR rhs(s_start) != g(s_start))
    while (((!bheap_es_vacio(ir->cp) && 
    (comp_keys(((PosConClave*)bheap_minimo(ir->cp))->key, 
    obt_key(coordRobot, ir))) < 0))
    || ((ir->mapaInterno[ir->x][ir->y].rhs) != (ir->mapaInterno[ir->x][ir->y].g)) 
  ) {   
        // Extraemos el tope de la cola de prioridad
        u = ((PosConClave*)bheap_minimo(ir->cp))->coord;
        k_old = ((PosConClave*)bheap_minimo(ir->cp))->key;
        bheap_eliminar_minimo(ir->cp);

        if (comp_keys(k_old, obt_key(u, ir)) < 0) {
            PosConClave tmp = {u, obt_key(u, ir)};
            bheap_insertar(ir->cp, &tmp);
            assert(es_bheap(ir->cp) == 1);
        } 

        else if (ir->mapaInterno[u.x][u.y].g > ir->mapaInterno[u.x][u.y].rhs) {
             //fprintf(stderr, "Es sobreconsistente (g > rhs). Poner g = rhs\n");
            // Hacer nodo consistente (se actualizan las referencias al nodo en el mapa interno)
            ir->mapaInterno[u.x][u.y].g = ir->mapaInterno[u.x][u.y].rhs;
            int adyCount = 0;
            EstadoConCoord* ady = obt_ady2(ir, u, &adyCount);
            
            //fprintf(stderr,"nodo (%d,%d), ady count: %d\n", u.x, u.y,adyCount);

            for (int h = 0; h < adyCount; h++) {
                /*fprintf(stderr,"updatevertex en (%d,%d)\n", u.x, u.y);
                fprintf(stderr,"coords: %d,%d\n", ady[h].c.x,ady[h].c.y);
                fprintf(stderr,"est: %d,%d, %d\n", ady[h].est.g,ady[h].est.rhs,
                ady[h].est.tipoCelda);*/
                UpdateVertex(ady[h].est, ady[h].c, ir);
            }
            free(ady);
        } 
        else {
              //fprintf(stderr, "No es sobreconsistente (g <= rhs), poner g = inf\n");
            ir->mapaInterno[u.x][u.y].g = infty(ir);

            UpdateVertex(ir->mapaInterno[u.x][u.y], u, ir);
            int adyCount = 0;
            EstadoConCoord* ady = obt_ady2(ir, u, &adyCount);
            for (int h = 0; h < adyCount; h++) {
                UpdateVertex(ady[h].est, ady[h].c, ir);
            }
            free(ady);
    }
}

}

void actualizar_segun_direccion(InfoRobot* ir, int dist, int dx, int dy, Coord last) {
    
    // Actualizar casillas validas descubiertas
    for (int h = 1; h < dist; h++) {
        if (ir->mapaInterno[ir->x + (h * dx)][ir->y + (h * dy)].tipoCelda == DESCONOCIDO)
            ir->mapaInterno[ir->x + (h * dx)][ir->y + (h * dy)].tipoCelda = VALIDO;
    }
    // Detecto obstaculo, actualizar
    if (dist <= ir->distSensorConocida) {

        //fprintf(stderr,"obstaculo detectado\n");
        // Actualizar solamente si la celda no excede los limites del mapa
        if ( (ir->y + (dist * dy)) >= 0 && (ir->y + (dist * dy)) < (ir->M) &&
        (ir->x + (dist * dx)) >= 0 && (ir->x + (dist * dx)) < (ir->N ) ) {
            
            int indXObstaculo = ir->x + (dist * dx);
            int indYObstaculo = ir->y + (dist * dy);
            ir->mapaInterno[indXObstaculo][indYObstaculo].tipoCelda = OBSTACULO;

            Coord coordRobot = {ir->x, ir->y};
            ir->km += dist_manhattan(last, coordRobot);
            last.x = ir->x; last.y = ir->y;

             // fprintf(stderr, "casilla: %d,%d\n", indXObstaculo, indYObstaculo);

            /*
            Para esta implementacion del algoritmo, basta con actualizar el
            nodo que posee el obstaculo.
            */
            Coord coordObstaculo = {indXObstaculo, indYObstaculo};
            UpdateVertex(ir->mapaInterno[indXObstaculo][indYObstaculo], coordObstaculo, ir);

      


            // Recalcular ruta optima
            CalcularRutaOptima(ir);
        }
    }
}

void actualizar_mapa_interno(InfoRobot* ir, int* d, Coord last) {
    actualizar_segun_direccion(ir, d[0], -1, 0, last); // arriba
    actualizar_segun_direccion(ir, d[1], 1, 0, last); // abajo
    actualizar_segun_direccion(ir, d[2], 0, -1, last); // izq
    actualizar_segun_direccion(ir, d[3], 0, 1, last); // der
}

int siguiente_movimiento(InfoRobot* ir, EstadoConCoord* posibles) {
    int adyCount = 0, posiblesMov = 0;
    Coord coordRobot = {ir->x, ir->y};
    EstadoConCoord* ady = obt_ady2(ir, coordRobot, &adyCount);
    
    int min = infty(ir);

    for (int h = 0; h < adyCount; h++) {

        int v = suma_inf(costo_movimiento(ir, ir->mapaInterno[ir->x][ir->y],
        ady[h].est), ady[h].est.g, ir);
 
        if (v < min) {
            min = v;
            posibles[0].est = ady[h].est;
            posibles[0].c = ady[h].c;
            posiblesMov = 1;
        } else if (v == min) {
            posibles[1].est = ady[h].est;
            posibles[1].c = ady[h].c;
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