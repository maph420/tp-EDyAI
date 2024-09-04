#ifndef __ROBOT_UTILS_H__
#define __ROBOT_UTILS_H__
#include "estr/bheap.h"

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
} Coord;

typedef struct {
    int g;
    int rhs; // rhs: estim
    TipoCelda tipoCelda;
} Estado;

typedef struct {
    Coord coord;
    Key key;
} PosConClave;


typedef struct {
    Estado est;
    Coord c;
} EstadoConCoord;

typedef struct {
    int x, y; // Pos actual
    int i1, i2, j1, j2; // Arranca en (i1,j1) y destina a (i2,j2)
    BHeap cp;
    int N, M;
    int distSensorConocida;
    Estado** mapaInterno;
    char* rastro;
    int km;
} InfoRobot;

void imprime_nodo(void*);

void impr_mapa(InfoRobot*);

int aleatorio();

int infty(InfoRobot* ir);

/** 
 * Tomando como infinito el definido en infty(), 
 * realiza una suma respetando las propiedades
 * matematicas del infinito.
 */
int suma_inf(int, int, InfoRobot* ir);

/**
 * Toma el maximo elemento de una lista l.
 */
int max(int*, int);

/**
 * Calcula la distancia de manhattan entre
 * dos puntos.
 */
int dist_manhattan(Coord, Coord);

/**
 * Calcula el valor g del punto actual.
 */
int g_val(InfoRobot*, Coord);


/* El costo entre dos estados es infinito si alguno es un obstaculo 
(imposible moverse entre ellos). En otro caso, es 1. No consideramos
movimientos en diagonal, por lo que se descarta que el costo entre dos
estados validos pueda ser sqrt(2)*/
int costo_movimiento(InfoRobot*, Estado, Estado);

/**
 * Funcion de comparacion de keys de prioridad.
 * Por orden lexicografico.
 */
int comp_keys(Key, Key);

int compara_estado_con_clave(void*, void*) ;

// todo: cambiar p que no tome estado
/**
 * Calcula la clave de prioridad del estado pasado.
 */
Key obt_key(Coord, InfoRobot*);

/**
 * Funcion auxiliar para retornar un estado adyacente al
 * estado actual, en la direccion dada
 */
int asigna_adyacencia(Estado*, Coord, InfoRobot*, int, int, int);

/**
 * Retorna un array de (a lo sumo) 4 elementos, que son los
 * estados adyacentes al pasado.
 */
Estado* obt_ady(InfoRobot*, Coord, int*);

EstadoConCoord* obt_ady2(InfoRobot*, Coord, int*);

/**
 * Asigna la memoria para las estructuras utilizadas por
 * el robot.
 */
void InicializaRobot(InfoRobot*); 

/**
 * Actualiza el valor-g estimado del nodo. De ser necesario,
 * actualiza la cola de prioridad con el nuevo valor estimado.
 */
void UpdateVertex(Estado, Coord, InfoRobot*);

/**
 * Se llama la primera vez, o bien cuando algun obstaculo
 * bloquea la ruta optima previamente calculada. Usa la cola
 * de prioridad de nodos inconsistentes y a medida que los convierte
 * en consistente, actualiza las estimaciones de sus vecinos, hasta que
 * eventualmente los estados se expanden hasta el inicio.
 */
void CalcularRutaOptima(InfoRobot*);

/**
 * Mueve el robot en 1 posicion, segun cual sea el 
 * siguiente estado a recorrer. Ademas, actualiza el
 * rastro del robot.
 * se asume que (ir->x, ir->y) != (sig.x, sig.y)
 */

int mover_robot(InfoRobot*, Coord, int);

/**
 * Determina el siguiente (o siguientes) movimiento(s).
 * Decide buscando el estado adyacente cuyo valor-g sumado
 * al costo de moverse al mismo sea mínimo.
 */
int siguiente_movimiento(InfoRobot*, EstadoConCoord*);

/**
 * Funcion auxiliar para actualizar el mapa interno del robot
 * en una direccion del sensor dada.
 */
void actualizar_segun_direccion(InfoRobot*, int, int, int, Coord);

/**
 * Actualiza el mapa interno del robot segun la nueva informacion
 * recabada en el arreglo de direcciones devuelto por el sensor.
 */
void actualizar_mapa_interno(InfoRobot*, int*, Coord);

#endif /* __ROBOT_UTILS_H__ */