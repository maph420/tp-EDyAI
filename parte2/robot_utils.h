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
    TipoCasilla tipoCasilla;
} State;

typedef struct {
    State est;
    Key key;
} EstadoConClave;

typedef struct {
    int x, y; // pos actual
    int i1, i2, j1, j2; // arranca en (i1,j1) y destina a (i2,j2)
    BHeap cp;
    int N, M; //sabe
    int distSensorConocida; // sabe solo lo que puede inferir de los result de los escaneos
    State** mapaInterno;
    char* rastro;
} InfoRobot;

void imprime_nodo(void*);

void impr_mapa(InfoRobot*);

int infty();

int aleatorio();

int suma_inf(int, int);

int max(int*, int);

int heuristica(Nodo, Nodo);

int g_val(InfoRobot*, Nodo);

int costo_movimiento(InfoRobot*, State, State);

EstadoConClave* crea_estado_con_clave(Nodo, Key, int, int, int) ;

int comp_keys(Key, Key);

int compara_estado_con_clave(void*, void*) ;
// todo: cambiar p que no tome estado
Key obt_key(State, State);

State* obt_ady(InfoRobot*, State, int*);

void inicializa(InfoRobot*);

void UpdateVertex(State, InfoRobot*);

void ComputeShortestPath(InfoRobot*);

int mover_robot(InfoRobot*, Nodo, int);

int siguiente_movimiento(InfoRobot*, State*);

void actualizar_segun_direccion(InfoRobot*, int, int, int, int*, int);

void actualizar_mapa_interno(InfoRobot*, int*, int);

#endif