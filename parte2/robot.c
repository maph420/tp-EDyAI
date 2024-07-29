#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "estr/heap.h"

// Definición de las estructuras de datos
typedef struct {
    int x, y; // Posición del nodo
} Node;


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
    int visitado;
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
    double min_g_rhs = min(s.g, s.rhs);
    k.key1 = min_g_rhs + heuristic(start.node, s.node);
    k.key2 = min_g_rhs;
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

double cost(Node a, Node b) {
    return dist(a.x, b.x, a.y, b.y);
}

int compara_estado(void* rs1, void* rs2) {
    State s1 = *(State*)rs1;
    State s2 = *(State*)rs2;
    if (s1.k.key1 < s2.k.key1) return 1;
    if (s1.k.key1 > s2.k.key1) return -1;
    return (s1.k.key2 < s2.k.key2) ? 1 : -1;
}

int comp_key(Key kA, Key kB) {
    if (kA.key1 < kB.key1) return 1;
    if (kA.key1 > kB.key1) return -1;
    return kA.key2 < kB.key2 ? 1 : -1;
}

double c(int obstaculo) {
    return obstaculo? DBL_MAX : 1;
}

State inicializa(InfoRobot* ir) {
    ir->cp = bheap_crear(ir->N * ir->M, compara_estado);
    ir->mapaInterno = malloc(sizeof(State) * ir->N);
    int km = 0;
    for (int i = 0; i < ir->N; i++) {
        ir->mapaInterno[i] = malloc(sizeof(State) * ir->M);
        for (int j = 0; j < ir->M; j++) {
            ir->mapaInterno[i][j].rhs = DBL_MAX;
            ir->mapaInterno[i][j].g = DBL_MAX;
            ir->mapaInterno[i][j].visitado = (i == ir->j2 && j == ir->i2);
        }
    }
    
    State ini;
    printf("jijodebu\n");
    ini.node.x = ir->i1; ini.node.y = ir->j1;
    ir->mapaInterno[ir->j2][ir->i2].k = 
    calcular_key(ir->mapaInterno[ir->j2][ir->i2], ini);
    bheap_insertar(ir->cp, &(ir->mapaInterno[ir->j2][ir->i2]));

    return ini;
}


State* obt_suc(InfoRobot* ir, State curr, int* sucCount) {
    State* sucesores = malloc(sizeof(State) * 4);
    if (curr.node.y < ir->N) {
        sucesores[*sucCount].node.x = curr.node.x;
        sucesores[*sucCount].node.y = curr.node.y + 1;
        (*sucCount)++;
    }
    if (curr.node.y > 0) {
        sucesores[*sucCount].node.x = curr.node.x;
        sucesores[*sucCount].node.y = curr.node.y - 1;
        (*sucCount)++;
    }
    if (curr.node.x < ir->M) {
        sucesores[*sucCount].node.x = curr.node.x + 1;
        sucesores[*sucCount].node.y = curr.node.y;
        (*sucCount)++;
    }
    if (curr.node.x > 0) {
        sucesores[*sucCount].node.x = curr.node.x - 1;
        sucesores[*sucCount].node.y = curr.node.y;
        (*sucCount)++;
    }
    return sucesores;
}

//c(u, s')?
void UpdateVertex(State u, InfoRobot* ir, State ini) {
    if (u.node.x != ir->i2 && u.node.y != ir->j2) {
        int sucCount = 0;
        State* sucs = obt_suc(ir, u, &sucCount);
        printf("obtuvo suc\n");
        double min = DBL_MAX; 
        for (int h = 0; h < sucCount; h++) {
            double v = sucs[sucCount].c +
            sucs[h].g;
            if (v < min) min = v;
        }
    }
    printf("hola\n");
    bheap_buscar_eliminar(ir->cp, &u);

printf("if\n"); 
    if (u.rhs != u.g) {
        u.k = calcular_key(u, ini);
        bheap_insertar(ir->cp, &u);
        printf("insertadfo\n");
    }

}

void ComputeShortestPath(InfoRobot* ir, State ini) {
    BHeap cp = ir->cp;

    while (ini.rhs != ini.g || 
    comp_key(((State*)bheap_maximo(ir->cp))->k, calcular_key(ini, ini)) < 0 ) {
        State u = *(State*)bheap_maximo(ir->cp);
        ir->cp = bheap_eliminar_maximo(ir->cp);

        if (u.g > u.rhs) {
            u.g = u.rhs;
            int cantPred = 0;
            State* pred = obt_suc(ir, u, &cantPred);
            for (int h = 0; h < cantPred; h++) {
                UpdateVertex(pred[h], ir, ini);
            }
        } 
        else {
             printf("else\n");
            u.g = DBL_MAX;
            int cantPred = 0;
            State* pred = obt_suc(ir, u, &cantPred);
            printf("obtuvo\n");
            for (int h = 0; h < cantPred; h++) {
                UpdateVertex(pred[h], ir, ini);
            }
            printf("sale del for\n");
            UpdateVertex(u, ir, ini);
    }
}
}


int main(int argc, char** argv) {
    InfoRobot* ir = malloc(sizeof(InfoRobot));
    ir->N = 5; ir->M = 6; 
    ir->i1 = 0; ir->j1 = 0;
    ir->i2 = 5; ir->j2 = 4;
    State inicial = inicializa(ir);

    ComputeShortestPath(ir, inicial);


    return 0;
}