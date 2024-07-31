#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "estr/heap.h"

// Definición de las estructuras de datos
typedef struct {
    int x, y; // Posición del nodo
} Node;

typedef enum {
    SIN_VISITAR,
    VISITADO,
    OBSTACULO
} EstadoCasilla;

typedef struct {
    int key1;
    int key2;
} Key;

typedef struct {
    Node node;
    int g;
    int rhs;
    int h; // Función heurística
    Key k;
    int c;
    EstadoCasilla est;
} State;

typedef struct {
    int x, y; // pos actual
    int i1, i2, j1, j2; // arranca en (i1,j1) y destina a (i2,j2)
    BHeap cp;
    int N, M;
    State** mapaInterno;
    char* rastro;
} InfoRobot;

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
    k.key1 = min_g_rhs + heuristic(start.node, s.node);
    k.key2 = min_g_rhs;
    //printf("retorna (%d, %d)\n", k.key1, k.key2);
    return k;
} 

int g_val(InfoRobot* ir, Node n) {
    //printf("d((%d, %d), (%d, %d))\n", n.x, n.y, 
     //ir->mapaInterno[ir->j1][ir->i1].node.x, 
     //ir->mapaInterno[ir->j1][ir->i1].node.y);

    return heuristic(n, ir->mapaInterno[ir->j2][ir->i2].node);
}

void imprime_nodo(void* refNodo) {
    State s = *(State*)refNodo;
    printf("(%d, %d); key: (%d, %d)\n", s.node.x, s.node.y, s.k.key1, s.k.key2);
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

State* crea_estado(Node n, Key k) {
    State* s = malloc(sizeof(State)) ;
    s->node.x = n.x;
    s->node.y = n.y;
    s->k.key1 = k.key1;
    s->k.key2 = k.key2;
    return s;
}

int cost(InfoRobot* ir, State s1, State s2) {
    return (ir->mapaInterno[s1.node.x][s1.node.y].est == OBSTACULO
    || ir->mapaInterno[s2.node.x][s2.node.y].est == OBSTACULO) ? 100 : 1;
}

void impr_mapa(InfoRobot* ir) {
    printf("(estado, g, rhs)\n");
    for (int i = 0; i < ir->N; i++) {
        for (int j = 0; j < ir->M; j++) {
            printf("(%d, %d, %d)\t ",
            ir->mapaInterno[i][j].est, 
            ir->mapaInterno[i][j].g,
            ir->mapaInterno[i][j].rhs);
        }
        printf("\n");
    }
}

void inicializa(InfoRobot* ir) {
    ir->cp = bheap_crear(ir->N * ir->M, compara_estado);
    ir->mapaInterno = malloc(sizeof(State) * ir->N);

    for (int i = 0; i < ir->N; i++) {
        ir->mapaInterno[i] = malloc(sizeof(State) * ir->M);
        for (int j = 0; j < ir->M; j++) {
            ir->mapaInterno[i][j].node.x = i;
            ir->mapaInterno[i][j].node.y = j;
            ir->mapaInterno[i][j].rhs = 100; 
            ir->mapaInterno[i][j].g = 100;
            ir->mapaInterno[i][j].c = 1;
            ir->mapaInterno[i][j].est = (i == ir->j2 && j == ir->i2) ? VISITADO : SIN_VISITAR;
        }
    }
    
    printf("jijodebu\n");

    ir->mapaInterno[ir->j2][ir->i2].rhs = 0;
    ir->mapaInterno[ir->j2][ir->i2].est = 1;
    Key k = calcular_key(ir->mapaInterno[ir->j2][ir->i2], ir->mapaInterno[ir->j1][ir->i1]);

    ir->cp = bheap_insertar(ir->cp,
     crea_estado(ir->mapaInterno[ir->j2][ir->i2].node, k));

    printf("sef\n");
}

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

    State* est = crea_estado(u.node, calcular_key(u, ir->mapaInterno[ir->j1][ir->i1]));
    
    printf("Current: (%d, %d)\n", est->node.x, est->node.y);

    // u != ini i.e. (x,y) != (j2,i2)
    if (u.node.y != ir->i2 || u.node.x != ir->j2) {
        int sucCount = 0;
        State* sucs = obt_ady(ir, u, &sucCount);

        //printf("obtuvo suc\n");
        int minVal = 
        cost(ir, ir->mapaInterno[sucs[0].node.x][sucs[0].node.y], 
        ir->mapaInterno[u.node.x][u.node.y]) + 
        ir->mapaInterno[sucs[0].node.x][sucs[0].node.y].g;
        

            //printf("g: %d\n", ir->mapaInterno[sucs[0].node.x][sucs[0].node.y].g);
            printf("Sucesor %d: (%d, %d)\n",
            0, 
            sucs[0].node.x,
            sucs[0].node.y
            );

        for (int h = 1; h < sucCount; h++) {
            //printf("g: %d\n", ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].g);
            printf("Sucesor %d: (%d, %d)\n",
            h, 
            sucs[h].node.x,
            sucs[h].node.y
            );
            //printf("c(s, s'): %d, g(s): %d\n", cost(ir, ir->mapaInterno[sucs[h].node.x][sucs[h].node.y], 
            //ir->mapaInterno[u.node.x][u.node.y]), ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].g);

            // aca
            int v = cost(ir, ir->mapaInterno[sucs[h].node.x][sucs[h].node.y], 
            ir->mapaInterno[u.node.x][u.node.y]) +
            ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].g;

            if (v < minVal) {
                minVal = v;
            }
        }
        // siguiente nodo a recorrer
        printf("Posible rhs del predecesor actual: %d\n", minVal);
        ir->mapaInterno[u.node.x][u.node.y].rhs = minVal;
        est->k = calcular_key(ir->mapaInterno[u.node.x][u.node.y], ir->mapaInterno[ir->j1][ir->i1]);
        free(sucs);
    } else printf("Es el nodo meta. No hace nada\n");

    //printf("buscar/eliminar: ");
    //imprime_nodo((State*)est);


printf("Se ejecuta buscar y elim (%d, %d)\n", u.node.x, u.node.y);
    int e = bheap_buscar_eliminar(ir->cp, &u);
    printf("%s\n", e>0? "Se encontro y elimino" : "No encontrado");


    if (ir->mapaInterno[u.node.x][u.node.y].rhs != 
    ir->mapaInterno[u.node.x][u.node.y].g) {
        printf("Insertado en el heap\n");
        ir->cp = bheap_insertar(ir->cp, est);
    }

}

int comp_key(Key kA, Key kB) {
    if (kA.key1 < kB.key1) return -1;
    if (kA.key1 > kB.key1) return 1;
    return (kA.key2 < kB.key2) ? -1 : (kA.key2 > kB.key2) ? 1 : 0;
}

void ComputeShortestPath(InfoRobot* ir) {
    int count = 0;

// Pred(curr): conj casillas desde las cuales puedo llegar a curr
// Suc(curr): conj casillas a las que puedo llegar desde curr
// 4 puntos cardinales, impedimentos? obstaculo en alguna direccion.

    //printf("Llave tope: (%.1lf, %.1lf)\n", llaveTope.key1, llaveTope.key2)

// while (U.TopKey() < CalculateKey(s_start) OR rhs(s_start) != g(s_start))
    while (((!bheap_es_vacio(ir->cp) && (comp_key(((State*)bheap_minimo(ir->cp))->k, 
    calcular_key(ir->mapaInterno[ir->j1][ir->i1], ir->mapaInterno[ir->j1][ir->i1]))) < 0)) 
    || (ir->mapaInterno[ir->j1][ir->i1].rhs) != (ir->mapaInterno[ir->j1][ir->i1].g)) {
        

        printf("----\nMAPA:\n----\n"); impr_mapa(ir);

        printf("----\nBHEAP:\n----\n"); bheap_recorrer(ir->cp, imprime_nodo);
        
        // popear el maximo elemento de la cola
        State u = *(State*)bheap_minimo(ir->cp);
        ir->cp = bheap_eliminar_minimo(ir->cp);
        printf("Key top: (%d, %d) ", u.k.key1, u.k.key2);
        Key llave = calcular_key(ir->mapaInterno[ir->j1][ir->i1], ir->mapaInterno[ir->j1][ir->i1]);
        printf("vs Key inicial: (%d, %d)\n", llave.key1, llave.key2);

        printf("comp keys -> %d\n", comp_key(u.k, llave));
        // no descubierto
        if (ir->mapaInterno[u.node.x][u.node.y].g > 
        ir->mapaInterno[u.node.x][u.node.y].rhs) {
        printf("Es sobreconsistente. Poner g = rhs\n");
            ir->mapaInterno[u.node.x][u.node.y].g = ir->mapaInterno[u.node.x][u.node.y].rhs;
            int cantPred = 0;
            State* pred = obt_ady(ir, u, &cantPred);
            for (int h = 0; h < cantPred; h++) {
                printf("---\nPredecesor %d: (%d, %d)\n",
                h,
                pred[h].node.x,
                pred[h].node.y);
                UpdateVertex((pred[h]), ir);
                printf("---\n");
            }
        } 
        else {
            printf("No es sobreconsistente\n");
            ir->mapaInterno[u.node.x][u.node.y].g = 600;
            UpdateVertex(ir->mapaInterno[u.node.x][u.node.y], ir);

            int cantPred = 0;
            State* pred = obt_ady(ir, u, &cantPred);
            for (int h = 0; h < cantPred; h++) {
                printf("---\nPredecesor %d: (%d, %d)\n",
                h,
                pred[h].node.x,
                pred[h].node.y);
                UpdateVertex((pred[h]), ir);
                printf("---\n");
            }
            printf("sale del for\n");
    }

    //if (count >= 30) {printf("count: %d\n", count); break;}
    //count++;
     printf("rhs ini %d, g ini %d\n",
        ir->mapaInterno[ir->j1][ir->i1].rhs,
        ir->mapaInterno[ir->j1][ir->i1].g) ;
       llave = calcular_key(ir->mapaInterno[ir->j1][ir->i1], ir->mapaInterno[ir->j1][ir->i1]);
        printf("Key inicial: (%d, %d)\n", llave.key1, llave.key2);
        printf("BHeap ahora: \n"); bheap_recorrer(ir->cp, imprime_nodo);
}
printf("sale del while\n");
printf("El mapa queda:\n"); impr_mapa(ir);

}

int main(int argc, char** argv) {
    InfoRobot* ir = malloc(sizeof(InfoRobot));
    ir->N = 5; ir->M = 6; 
    ir->i1 = 0; ir->j1 = 0;
    ir->i2 = 5; ir->j2 = 4;
    inicializa(ir);

    //impr_mapa(ir);

    ComputeShortestPath(ir);

    return 0;
}