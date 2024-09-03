#ifndef __ROBOT_UTILS_H__
#define __ROBOT_UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include "estr/pila.h"
#include "estr/tablahash.h"
#define LONGITUD_MAX_LINEA 1600
// chequear
#define FACTOR_CARGA_UMBRAL 0.75

// izquierda, derecha, arriba, abajo, invalida
typedef enum {
    IZQ,
    DER,
    ARR,
    ABA,
    INV
} Direccion;

typedef struct {
    int x, y; // pos actual
    int i1, i2, j1, j2; // arranca en (i1,j1) y destina a (i2,j2)
    TablaHash visitados;
    Pila camino;
    char* rastro;
} InfoRobot;

typedef struct {
    int x, y;
    unsigned int valido;
    Direccion dirOrigen;
} NodoMapa;


/**
 * Notifica si el robot se chocó o si se desplazó a una posición válida. 
 * Además, verifica que el robot no se salga de los limites del mapa, considerando 
 * esta situacion como un "choque".
 */
unsigned int movimiento_valido(char**, int, int, int, int);

/**
 * Retorna copia fisica de puntero a estructura NodoMapa
 */
void* nodomapa_copia(void *);

/**
 * Destruye estructura NodoMapa dada
 */
void nodomapa_destruir(void *);

/**
 * Comparacion entre estructuras NodoMapa. Mas detalles en el PDF
 */
int nodomapa_comparar(void*, void*);

/**
 * Genera aleatorio entre 0 y 1
 */
int aleatorio();

void inicializa_robot(InfoRobot*, int);

int sig_nodo_x(Direccion, int);

int sig_nodo_y(Direccion, int);

/**
 * Retorna la direccion opuesta a la pasada. Utilizada para
 * regristrar el backtracking
 */
Direccion opuesta(Direccion);


Direccion obtener_direccion(InfoRobot*, char**, unsigned, unsigned, Direccion, Direccion*);

char asignar_direccion(Direccion);


void movimiento_robot(InfoRobot*, char**, unsigned int, unsigned int);


#endif /* __ROBOT_UTILS_H__ */