#ifndef FUNC_H
#define FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "estr/avl.h"
#include "estr/pila.h"


typedef unsigned int (*FuncionValidadora)(int** mapa, int x, int y) ;

// estructuras

typedef struct {
    int i1, i2, j1, j2;
    FuncionValidadora f;
} InfoRobot;

typedef struct {
    int x, y;
    unsigned int valido;
} NodoMapa;

// prototipos
/**
 * Retorna copia fisica de puntero a estructura NodoMapa
 */
void* nodomapa_copia(void *);

/**
 * Destruye estructura NodoMapa dado su puntero
 */
void nodomapa_destruir(void *);

/**
 * Comparacion entre estructuras NodoMapa. Mas detalles en el PDF
 */
int nodomapa_comparar(void*, void*);

/**
 * Imprime los campos del nodo NodoMapa
 */
static void imprimir_nodo(void*);

/**
 * Toma el nombre del archivo, los 3 datos que el robot no sabe, y la estructura InfoRobot,
 * donde se guardaran los campos de la informacion que el robot si maneja
 */
int validar_arch_y_guardar_info(char*, int***, unsigned int*, unsigned int*, InfoRobot*);

/**
 * Funcion a la que recurre el robot al realizar un movimiento. Notifica si
 * el robot se chocó o si se desplazó a una posición válida. Además, verifica que
 * el robot no se salga de los limites del mapa, considerando "choque" esta situacion.
 */
unsigned int movimiento_valido(int**, int, int);

/**
 * testeo (borrador)
 */
void test_avl();

/**
 * testeo (borrador)
 */
void test_pila();


#endif