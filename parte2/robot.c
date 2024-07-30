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
    double key1;
    double key2;
} Key;

typedef struct {
    Node node;
    double g;
    double rhs;
    double h; // Función heurística
    Key k;
    double c;
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

// prototipos
double heuristic(Node a, Node b);
double min(double a, double b);

// Implementación de la función key
Key calcular_key(State s, State start) {
    printf("calcula llave\n");
    Key k;
    printf("rhs: %.1lf\n", s.rhs);
    double min_g_rhs = min(s.g, s.rhs);
    k.key1 = min_g_rhs + heuristic(start.node, s.node);
    k.key2 = min_g_rhs;
    //printf("retorna (%.1lf, %.1lf)\n", k.key1, k.key2);
    return k;
}

double min(double a, double b) {
    return (a < b) ? a : b;
}

// heurística (distancia Manhattan)
double heuristic(Node a, Node b) {
    return fabs(a.x - b.x) + fabs(a.y - b.y);
}

double dist (long x1, long y1, long x2, long y2) {
    return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}

double g_val(InfoRobot* ir, Node n) {
    return dist(n.x, n.y,
     ir->mapaInterno[ir->j1][ir->i1].node.x, 
     ir->mapaInterno[ir->j1][ir->i1].node.y) ;
}

void imprime_nodo(void* refNodo) {
    State s = *(State*)refNodo;
    printf("(%d, %d); key: %.1lf\n", s.node.x, s.node.y, s.k.key1);
}

int compara_estado(void* rs1, void* rs2) {
    State s1 = *(State*)rs1;
    State s2 = *(State*)rs2;
    if (s1.k.key1 < s2.k.key1) return -1;
    if (s1.k.key1 > s2.k.key1) return 1;
    return (s1.k.key2 < s2.k.key2) ? -1 : (s1.k.key2 > s2.k.key2) ? 1 : 0;
}

double cost(InfoRobot* ir, State s) {
    return ir->mapaInterno[s.node.x][s.node.y].est == OBSTACULO ? 100 : 1;
}

void impr_mapa(InfoRobot* ir) {
    printf("(g, h, rhs)\n");
    for (int i = 0; i < ir->N; i++) {
        for (int j = 0; j < ir->M; j++) {
            printf("(%.0lf, %.0lf, %.0lf)\t ", 
            ir->mapaInterno[i][j].g,
            ir->mapaInterno[i][j].h,
            ir->mapaInterno[i][j].rhs);
        }
        printf("\n");
    }
}


State inicializa(InfoRobot* ir) {
    ir->cp = bheap_crear(ir->N * ir->M, compara_estado);
    ir->mapaInterno = malloc(sizeof(State) * ir->N);
    int km = 0;
    for (int i = 0; i < ir->N; i++) {
        ir->mapaInterno[i] = malloc(sizeof(State) * ir->M);
        for (int j = 0; j < ir->M; j++) {
            ir->mapaInterno[i][j].node.x = i;
            ir->mapaInterno[i][j].node.y = j;
            ir->mapaInterno[i][j].rhs = 100;
            //ir->mapaInterno[i][j].g = g_val(ir, ir->mapaInterno[i][j].node); 
            ir->mapaInterno[i][j].g = 100;
            ir->mapaInterno[i][j].c = cost(ir, ir->mapaInterno[i][j]);
            ir->mapaInterno[i][j].est = (i == ir->j2 && j == ir->i2) ? VISITADO : SIN_VISITAR;
        }
    }
    
    
    printf("jijodebu\n");

    ir->mapaInterno[ir->j2][ir->i2].h = 
    heuristic(ir->mapaInterno[ir->j2][ir->i2].node, ir->mapaInterno[ir->j1][ir->i1].node);
    ir->mapaInterno[ir->j2][ir->i2].rhs = 0;
    ir->mapaInterno[ir->j2][ir->i2].k = 
    calcular_key(ir->mapaInterno[ir->j2][ir->i2], ir->mapaInterno[ir->j1][ir->i1]);
    bheap_insertar(ir->cp, &(ir->mapaInterno[ir->j2][ir->i2]));

    return ir->mapaInterno[ir->j1][ir->i1];
}

State* obt_ady(InfoRobot* ir, State curr, int* adyCount) {
    State* adyacentes = malloc(sizeof(State) * 4);
    /*printf("curr x-> %d\n", curr.node.x);
    printf("curr y-> %d\n", curr.node.y);
    printf("N-1 -> %d\n", ir->N -1);
    printf("M-1 -> %d\n", ir->M -1);*/

    if (curr.node.y < ir-> M - 1) {
        adyacentes[*adyCount].node.x = curr.node.x;
        adyacentes[*adyCount].node.y = curr.node.y + 1;
        (*adyCount)++;
    } else printf("no entra\n");
    if (curr.node.y > 0) {
        adyacentes[*adyCount].node.x = curr.node.x;
        adyacentes[*adyCount].node.y = curr.node.y - 1;
        (*adyCount)++;
    } else printf("%d <= 0\n", curr.node.y);
    if (curr.node.x < ir-> N - 1) {
        printf("curr x %d, curr y %d\n", curr.node.x, curr.node.y);
        adyacentes[*adyCount].node.x = curr.node.x + 1;
        adyacentes[*adyCount].node.y = curr.node.y;
        (*adyCount)++;
    } else printf("%d >= %d\n", curr.node.x, ir->M);
    if (curr.node.x > 0) {
        adyacentes[*adyCount].node.x = curr.node.x - 1;
        adyacentes[*adyCount].node.y = curr.node.y;
        (*adyCount)++;
    } else printf("%d <= 0\n", curr.node.x);
    return adyacentes;
}

//c(u, s')?
void UpdateVertex(State u, InfoRobot* ir, State ini) {
    printf("Current: (%d, %d)\n", u.node.x, u.node.y);
    double heu = heuristic(u.node, ir->mapaInterno[ir->j1][ir->i1].node);
    ir->mapaInterno[u.node.x][u.node.y].h = heu;
    //ir->mapaInterno[u.node.x][u.node.y].g = g_val(ir, u.node);
    //ir->mapaInterno[u.node.x][u.node.y].c = 1;

    if (u.node.x != ir->i2 || u.node.y != ir->j2) {
        int sucCount = 0;
        State* sucs = obt_ady(ir, u, &sucCount);
        State s;

        printf("obtuvo suc\n");
        double minVal = DBL_MAX; 
        for (int h = 0; h < sucCount; h++) {
            ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].g = 
            g_val(ir, sucs[h].node);
            ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].c = 
            cost(ir, u);

            printf("sucesor %d: (%d, %d)\n",
            h, 
            sucs[h].node.x,
            sucs[h].node.y
            );
            double v = ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].c +
            ir->mapaInterno[sucs[h].node.x][sucs[h].node.y].g;
   
            if (v < minVal) {
                minVal = v;
            }
        }
        printf("minval: %.1lf \n", minVal);
        // siguiente nodo a recorrer
        ir->mapaInterno[u.node.x][u.node.y].rhs = minVal;

    } else printf("todomal\n"); 
    
    ir->mapaInterno[u.node.x][u.node.y].k = calcular_key(ir->mapaInterno[u.node.x][u.node.y], ini);
    printf("buscar/eliminar: ");
    imprime_nodo((State*)&u);
    bheap_buscar_eliminar(ir->cp, &u) ;
    //int e = bheap_buscar_eliminar(ir->cp, &(ir->mapaInterno[u.node.x][u.node.y]));

    printf("rhs(u): %.1lf, g(u): %.1lf\n", u.rhs, u.g); 

    if (ir->mapaInterno[u.node.x][u.node.y].rhs != 
    ir->mapaInterno[u.node.x][u.node.y].g) {
        printf("Se inserta el nodo en la lista\n") ;
        bheap_insertar(ir->cp, &(ir->mapaInterno[u.node.x][u.node.y]));
    }

}

int comp_key(Key kA, Key kB) {
    if (kA.key1 < kB.key1) return -1;
    if (kA.key1 > kB.key1) return 1;
    return kA.key2 < kB.key2 ? -1 : (kA.key2 > kB.key2) ? 1 : 0;
}


void ComputeShortestPath(InfoRobot* ir, State ini) {
    BHeap cp = ir->cp;
    int count = 0;

// Pred(curr): conj casillas desde las cuales puedo llegar a curr
// Suc(curr): conj casillas a las que puedo llegar desde curr
// 4 puntos cardinales, impedimentos? obstaculo en alguna direccion.

    //printf("entra al while\n");
    //Key llaveTope = (*(State*)bheap_minimo(ir->cp)).k;

    //printf("Llave tope: (%.1lf, %.1lf)\n", llaveTope.key1, llaveTope.key2);
// while (U.TopKey() < CalculateKey(s_start) OR rhs(s_start) != g(s_start))
    while (ir->mapaInterno[ir->j1][ir->i1].rhs != ir->mapaInterno[ir->j1][ir->i1].g || 
    comp_key(((State*)bheap_minimo(ir->cp))->k, calcular_key(ini, ini)) < 0 ) {

        printf("BHEAP:\n");
        bheap_recorrer(ir->cp, imprime_nodo);
        
        // popear el maximo elemento de la cola
        State u = *(State*)bheap_minimo(ir->cp);
        ir->cp = bheap_eliminar_minimo(ir->cp);

        if (u.g > u.rhs) {
            ir->mapaInterno[u.node.x][u.node.y].g = ir->mapaInterno[u.node.x][u.node.y].rhs;
            int cantPred = 0;
            State* pred = obt_ady(ir, u, &cantPred);
            for (int h = 0; h < cantPred; h++) {
                printf("Predecesor %d: (%d, %d)\n",
                h,
                pred[h].node.x,
                pred[h].node.y);
                UpdateVertex((pred[h]), ir, ini);
            }
        } 
        else {
            printf("else\n");
            ir->mapaInterno[u.node.x][u.node.y].g = 100;
            UpdateVertex(u, ir, ini);

            int cantPred = 0;
            State* pred = obt_ady(ir, u, &cantPred);
            for (int h = 0; h < cantPred; h++) {
                printf("Predecesor %d: (%d, %d)\n",
                h,
                pred[h].node.x,
                pred[h].node.y);
                UpdateVertex((pred[h]), ir, ini);
            }
            printf("sale del for\n");
    }

    if (count >= 5) break;
    count++;
}
    impr_mapa(ir);
}


int main(int argc, char** argv) {
    InfoRobot* ir = malloc(sizeof(InfoRobot));
    ir->N = 5; ir->M = 6; 
    ir->i1 = 0; ir->j1 = 0;
    ir->i2 = 5; ir->j2 = 4;
    State inicial = inicializa(ir);

    impr_mapa(ir);


    ComputeShortestPath(ir, inicial);


    return 0;
}