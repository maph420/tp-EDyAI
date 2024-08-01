#ifndef __ROBOT_UTILS_H__
#define __ROBOT_UTILS_H__
#include "estr/heap.h"

// Definición de las estructuras de datos
typedef struct {
    int x, y; // Posición del nodo
} Node;

typedef enum {
    DESCONOCIDO,
    VISITADO,
    SIN_VISITAR_VALIDO,
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
    int N, M; //sabe? (preg)
    int d_max; // sabe? (preg)
    State** mapaInterno;
    char* rastro;
} InfoRobot;

int min(int a, int b) ;

int heuristic(Node a, Node b);

int g_val(InfoRobot* ir, Node n);

void imprime_nodo(void* refNodo);

int compara_estado(void* rs1, void* rs2) ;

State* crea_estado(Node n, Key k) ;

int cost(InfoRobot* ir, State s1, State s2);

void impr_mapa(InfoRobot* ir);

void inicializa(InfoRobot* ir);

State* obt_ady(InfoRobot* ir, State curr, int* adyCount);

void UpdateVertex(State u, InfoRobot* ir);

int comp_key(Key kA, Key kB);

void ComputeShortestPath(InfoRobot* ir);

Key calcular_key(State s, State start);

State siguiente_movimiento(InfoRobot* ir, int currX, int currY);

void actualizar_segun_direccion(InfoRobot* ir, int distancia, int dx, int dy);

void actualizar_mapa_interno(InfoRobot* ir, int* d) ;

int mover_robot(InfoRobot* ir, Node sig, int pasos);

#endif