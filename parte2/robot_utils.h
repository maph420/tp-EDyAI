#ifndef __ROBOT_UTILS_H__
#define __ROBOT_UTILS_H__
#include "estr/heap.h"

// Definición de las estructuras de datos

typedef enum {
    DESCONOCIDO,
    VALIDO,
    VISITADO,
    OBSTACULO
} TipoCasilla;

typedef struct {
    int id_1;
    int id_2;
} Key;

typedef struct {
    int x, y;
} Nodo;

typedef struct {
    Nodo nodo;
    int g;
    int rhs;
    Key k;
    TipoCasilla tipoCasilla;
} State;

typedef struct {
    int x, y; // pos actual
    int i1, i2, j1, j2; // arranca en (i1,j1) y destina a (i2,j2)
    BHeap cp;
    int N, M; //sabe
    int distSensorConocida; // NO sabe
    State** mapaInterno;
    char* rastro;
} InfoRobot;

int min(int, int) ;

int heuristic(Nodo, Nodo);

int g_val(InfoRobot*, Nodo);

void imprime_nodo(void*);

int compara_estado(void*, void*) ;

State* crea_estado(Nodo, Key) ;

int cost(InfoRobot*, State, State);

void impr_mapa(InfoRobot*);

void inicializa(InfoRobot*);

State* obt_ady(InfoRobot*, State, int*);

void UpdateVertex(State, InfoRobot*);

int comp_key(Key, Key);

void ComputeShortestPath(InfoRobot*);

Key calcular_key(State, State);

int siguiente_movimiento(InfoRobot*, State*);

void actualizar_segun_direccion(InfoRobot*, int, int, int, int*, int);

void actualizar_mapa_interno(InfoRobot*, int*, int);

int mover_robot(InfoRobot*, Nodo, int);

int infty();

int aleatorio();

int sum (int, int);

int max(int*, int);

#endif