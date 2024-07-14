#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "avl.h"
#include "pila.h"

#define LONGITUD_MAX_LINEA 255

typedef unsigned int (*FuncionValidadora)(int** mapa, int x, int y) ;

typedef struct {
    int i1, i2, j1, j2;
    FuncionValidadora f;
} InfoRobot;

typedef struct {
    int x, y;
    unsigned int valido;
} NodoMapa;


void* nodomapa_copia(void* n) {
    NodoMapa* copia = malloc(sizeof(NodoMapa)) ;
    copia->x = ((NodoMapa*)n)->x;
    copia->y = ((NodoMapa*)n)->y;
    copia->valido = ((NodoMapa*)n)->valido;
    return copia;
}

//(?)
void nodomapa_destruir(void* n) {
    free((NodoMapa*)n) ;
}

/**
 * Explicacion en pdf
 */
int nodomapa_comparar(void* refA, void* refB) {
    NodoMapa* a = (NodoMapa*)refA;
    NodoMapa* b = (NodoMapa*)refB;

    if (a->x < b->x || ( (a->x == b->x) && (a->y < b->y) )) return -1;
    if (a->x > b->x || ( (a->x == b->x) && (a->y > b->y) )) return 1;
    return 0;
}

static void imprimir_nodo(void* ref) {
    NodoMapa* nm = (NodoMapa*)ref;
    printf("(%d, %d)\tvalido: %d\n", nm->x, nm->y, nm->valido);
}


/**
 * Valida que el archivo respete el formato pedido, guarda el mapa del laberinto, asi
 * como sus dimensiones y le pasa a la estructura de info del robot las coords de inicio y fin
 *  */
// se asume: el laberinto dado tiene solucion, y los puntos de inicio y fin no son '#'
int validar_arch_y_guardar_info(char* nomArchivo, int*** mapa, unsigned int* nFilas, unsigned int* nCols, InfoRobot* ir) {

    char linea[LONGITUD_MAX_LINEA];
    FILE* archivoEntrada;
    unsigned int N, M, D, check, archivoValido = 1;
    int i1, i2, j1, j2;

    archivoEntrada = fopen(nomArchivo, "r");
    if (archivoEntrada == NULL) return 0;

    printf("Archivo '%s' abierto con exito\n", nomArchivo);
    for (unsigned int k = 0; (fgets(linea, LONGITUD_MAX_LINEA, archivoEntrada)) && archivoValido; k++) {
        // printf("k -> %d\n", k);
        switch (k) {
        case 0: {
            check = sscanf(linea, "%d %d %d\n", &N, &M, &D);
            if (check != 3)
                archivoValido = 0;
            else {
                *nFilas = N;
                *nCols = M;
                *mapa = malloc(sizeof(int*) * N);
                for (unsigned int h = 0; h < N; h++) 
                    (*mapa)[h] = malloc(M * sizeof(int));
            }
            break;
        }
        case 1:
            check = sscanf(linea, "%d %d\n", &i1, &j1);
            if (check != 2)
                archivoValido = 0;
            else {
                ir->i1 = i1;
                ir->j1 = j1;
            }
            break;
        case 2: 
            check = sscanf(linea, "%d %d\n", &i2, &j2);
            if (check != 2)
                archivoValido = 0;
            else {
                ir->i2 = i2;
                ir->j2 = j2;
            }
            break;
        default:
            // leer laberinto
            char* l;
            unsigned int j; 
            // la cant filas del archivo discrepta con el nro de filas pasado
            //printf("k-2: %d, N: %d\n", k-2, N);
            if (k-3 > N)
                archivoValido = 0;
            else {
                //printf("long linea: %ld, M: %d\n", strlen(linea), M);

                if (!(k-2 < N && strlen(linea)-1 == M))
                    archivoValido = 0;
                if (k-2 == N && strlen(linea) == M)
                    archivoValido = 1;
                //if ( k == M && ! (strlen(linea) == M))
                  //  archivoValido = 0;
                
                for (l = linea, j = 0; *l != '\n' && j < M && archivoValido; l++, j++) {
                    //printf("leido: %c\t", *l);
                    if (*l != '.' && *l != '#')
                        archivoValido = 0;
                    else {
                        (*mapa)[k-3][j] = (*l == '.');
                    }
                }
            }
            break;
        }
    }
    fclose(archivoEntrada);
    return archivoValido ;
}

// verifica si la casilla en (posX, posY) es una posicion valida (no es pared)
unsigned int movimiento_valido(int** mapa, int posX, int posY) {
    return mapa[posX][posY];
}

void test_avl() {
    AVL arbol = avl_crear(nodomapa_copia, nodomapa_comparar, nodomapa_destruir) ;
    int vals1[] = {10, 30, 20, 50, 40} ;
    int vals2[] = {55, 50, 30, 45, 35} ;
    int vals3[] = {0, 0, 1, 1, 0} ;
    NodoMapa* nm1 = malloc(sizeof(NodoMapa));
    NodoMapa* nm2 = malloc(sizeof(NodoMapa));
    NodoMapa* nm3 = malloc(sizeof(NodoMapa));
    NodoMapa* nm4 = malloc(sizeof(NodoMapa));
    NodoMapa* nm5 = malloc(sizeof(NodoMapa));
    nm1->x = vals1[0]; nm1->y = vals2[0]; nm1->valido = vals3[0];
    nm2->x = vals1[1]; nm2->y = vals2[1]; nm2->valido = vals3[1];
    nm3->x = vals1[2]; nm3->y = vals2[2]; nm3->valido = vals3[2];
    nm4->x = vals1[3]; nm4->y = vals2[3]; nm4->valido = vals3[3];
    nm5->x = vals1[4]; nm5->y = vals2[4]; nm5->valido = vals3[4];
    
    avl_insertar(arbol, nm1); avl_insertar(arbol, nm2); avl_insertar(arbol, nm3); 
    avl_insertar(arbol, nm4); avl_insertar(arbol, nm5);
 
    avl_recorrer(arbol, AVL_RECORRIDO_POST, imprimir_nodo);

}

void test_pila() {
    Pila p = pila_crear();
    
    int vals1[] = {10, 30, 20} ;
    int vals2[] = {55, 50, 30} ;
    int vals3[] = {0, 0, 1,} ;
    NodoMapa* nm1 = malloc(sizeof(NodoMapa));
    NodoMapa* nm2 = malloc(sizeof(NodoMapa));
    NodoMapa* nm3 = malloc(sizeof(NodoMapa));

    nm1->x = vals1[0]; nm1->y = vals2[0]; nm1->valido = vals3[0];
    nm2->x = vals1[1]; nm2->y = vals2[1]; nm2->valido = vals3[1];
    nm3->x = vals1[2]; nm3->y = vals2[2]; nm3->valido = vals3[2];

    pila_apilar(&p, nm1, nodomapa_copia);
    pila_apilar(&p, nm2, nodomapa_copia);
    pila_apilar(&p, nm3, nodomapa_copia);

    pila_imprimir(p, imprimir_nodo);

}


int main (int argc, char** argv) {

    // validacion argumentos
    if (argc != 2) {
        printf("Numero incorrecto de argumentos\n");
        return -1;
    }

    // variables para guardar info del archivo
    int** mapa = NULL;
    unsigned int numFilas, numCols;
    InfoRobot* infoRobot = malloc(sizeof(InfoRobot));
    infoRobot->f = movimiento_valido;

    int v = validar_arch_y_guardar_info(argv[1], &mapa, &numFilas, &numCols, infoRobot);

    if (!v) {
        printf("El formato del archivo no es valido (chequear que no haya salto de linea al final)\n");
        return -1;
    }
    printf("El archivo es valido\n");


    printf("(%d, %d) -> (%d, %d)\n", infoRobot->i1, infoRobot->j1, infoRobot->i2, infoRobot->j2);
    printf("MAPA:\n");
    for (unsigned int i=0; i < numFilas; i++) {
        for(unsigned int j = 0; j < numCols; j++)
            printf("%d", mapa[i][j]);
    printf("\n");
    }

    int m = movimiento_valido(mapa, 5, 6);
    printf("movimiento (5,6): %d\n", m);

    test_avl() ;

    test_pila() ;

    return 0;
}