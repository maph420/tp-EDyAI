#ifndef __TABLAHASH_H__
#define __TABLAHASH_H__

/** Retorna una copia fisica del dato */
typedef void *(*FuncionCopiadora)(void *dato);

/** Retorna un entero negativo si dato1 < dato2, 0 si son iguales y un entero
 * positivo si dato1 > dato2  */
typedef int (*FuncionComparadora)(void *dato1, void *dato2);

/** Libera la memoria reservada para el dato */
typedef void (*FuncionDestructora)(void *dato);

/** Funcion de hasheo */
typedef unsigned int (*FuncionHash)(void *dato);

typedef struct _TablaHash *TablaHash;


// Funciones auxiliares

/** 
 * Verifica si un numero es primo
*/
unsigned int es_primo(int n);

/** Funcion para hallar el numero primo
 * mas cercano por arriba al argumento dado.
*/
unsigned int primo_mas_cercano(int n);

/**
 * Crea una nueva tabla hash vacia, con la capacidad dada.
 */
TablaHash tablahash_crear(unsigned capacidad, FuncionCopiadora copia,
                          FuncionComparadora comp, FuncionDestructora destr,
                          FuncionHash hash);
/**
 * Retorna la capacidad de la tabla.
 */
int tablahash_capacidad(TablaHash tabla);

/**
 * Destruye la tabla.
 */
void tablahash_destruir(TablaHash tabla);

/**
 * Calcula el factor de carga alpha de la tabla
 */
float tablahash_factor_carga(TablaHash);

/**
 * Inserta un dato en la tabla, o lo reemplaza si ya se encontraba.
 */
void tablahash_insertar(TablaHash tabla, void *dato, float umbralMax);

/**
 * Retorna el dato de la tabla que coincida con el dato dado, o NULL si el dato
 * buscado no se encuentra en la tabla.
 */
int tablahash_buscar(TablaHash tabla, void *dato);

/**
 * Redimensiona la tabla hash duplicando su capacidad y
 * reposiciona todos los elementos.
 */
void tablahash_redimensionar(TablaHash tabla);

#endif /* __TABLAHASH_H__ */