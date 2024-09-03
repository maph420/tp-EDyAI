#ifndef __ROBOT_UTILS_H__
#define __ROBOT_UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // sacar
#include <math.h>
#include <time.h>
#include "estr/pila.h"
#include "estr/tablahash.h"
#define LONGITUD_MAX_LINEA 150
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
} NodoMapa;

/* Funcion hash usada para la tabla. 
(mas detalles en el informe)
*/
unsigned int hash(void*);

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
 * Notifica si el robot se chocó o si se desplazó a una posición válida. 
 * Además, verifica que el robot no se salga de los limites del mapa, considerando 
 * esta situacion como un "choque".
 */
unsigned int movimiento_valido(char**, int, int, int, int);

/**
 * Genera aleatorio entre 0 y 1
 */
int aleatorio();

/**
 * Asigna la memoria para las estructuras utilizadas por
 * el robot.
 */
void inicializa_robot(InfoRobot*, int);

/**
 * Modifica el valor de la abcisa segun la
 * direccion de movimiento.
 */
int sig_nodo_x(Direccion, int);

/**
 * Modifica el valor de la ordenada segun la
 * direccion de movimiento.
 */
int sig_nodo_y(Direccion, int);

/**
 * Retorna la direccion opuesta a la pasada. Utilizada para
 * regristrar el backtracking
 */
Direccion opuesta(Direccion);

/**
 * Obtiene la o las direcciones "preferidas" a las cuales el robot
 * se debe mover para llegar a la meta.
 * Se asume que el robot se encuentra parado en una posicion valida.
 */
Direccion obtener_direccion(InfoRobot*, char**, unsigned, unsigned, Direccion, Direccion*);

/**
 * Asigna el caracter correspondiente a la direccion dada.
 */
char asignar_direccion(Direccion);

/**
 * Lleva el robot a la meta, dada su posicion inicial.
 */
void movimiento_robot(InfoRobot*, char**, unsigned int, unsigned int, unsigned int, Direccion*);

#endif /* __ROBOT_UTILS_H__ */