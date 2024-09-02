#ifndef __ROBOT_UTILS_H__
#define __ROBOT_UTILS_H__
#include "estr/bheap.h"

// Definición de las estructuras de datos

typedef enum {
    DESCONOCIDO,
    VALIDO,
    VISITADO,
    OBSTACULO
} TipoCelda;

typedef struct {
    int id_1;
    int id_2;
} Key;

typedef struct {
    int x, y;
} Pos;

typedef struct {
    Pos pos;
    int g;
    int rhs; // rhs: estim
    TipoCelda tipoCelda;
} Estado;

typedef struct {
    Estado est;
    Key key;
} EstadoConClave;

typedef struct {
    int x, y; // pos actual
    int i1, i2, j1, j2; // arranca en (i1,j1) y destina a (i2,j2)
    BHeap cp;
    int N, M; //sabe
    int distSensorConocida; // sabe solo lo que puede inferir de los result de los escaneos
    Estado** mapaInterno;
    char* rastro;
    int km;
} InfoRobot;

void imprime_nodo(void*);

void impr_mapa(InfoRobot*);

int infty(InfoRobot* ir);

int aleatorio();

int suma_inf(int, int, InfoRobot* ir);

int max(int*, int);

int dist_manhattan(Pos, Pos);

int g_val(InfoRobot*, Pos);

int costo_movimiento(InfoRobot*, Estado, Estado);

int comp_keys(Key, Key);

int compara_estado_con_clave(void*, void*) ;
// todo: cambiar p que no tome estado
Key obt_key(Estado, InfoRobot*);

int asigna_adyacencias(Estado*, Estado, InfoRobot*, int, int, int);

Estado* obt_ady(InfoRobot*, Estado, int*);

void inicializa(InfoRobot*); // InicializaRobot

void UpdateVertex(Estado, InfoRobot*); // ActualizarCelda

void ComputeShortestPath(InfoRobot*); // CalcularRutaOptima

int mover_robot(InfoRobot*, Pos, int);

int siguiente_movimiento(InfoRobot*, Estado*);

void actualizar_segun_direccion(InfoRobot*, int, int, int, Estado);

void actualizar_mapa_interno(InfoRobot*, int*, Estado);

#endif /* __ROBOT_UTILS_H__ */