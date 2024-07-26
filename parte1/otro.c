#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#define LONGITUD_MAX_LINEA 255

typedef unsigned int (*FuncionValidadora)(int** mapa, int x, int y) ;

typedef struct {
    int i1, i2, j1, j2;
    FuncionValidadora f;
} InfoRobot;


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

double dist (long x1, long y1, long x2, long y2) {
    return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}


// verifica si la casilla en (posX, posY) es una posicion valida (no es pared)
unsigned int movimiento_valido(int** mapa, int posX, int posY) {
    return mapa[posX][posY];
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
    //mapa[0][3] = 4;
    for (unsigned int i=0; i < numFilas; i++) {
        for(unsigned int j = 0; j < numCols; j++) {
            /*
            printf("(%.1lf, %.1lf, %.1lf", dist(i,j, infoRobot->i1, infoRobot->j1),
            dist(i, j, infoRobot->i2, infoRobot->j2),
            dist(i,j, infoRobot->i1, infoRobot->j1) +
            dist(i, j, infoRobot->i2, infoRobot->j2));*/

            printf("(%.1lf)", dist(i,j, infoRobot->i1, infoRobot->j1) +
            dist(i, j, infoRobot->i2, infoRobot->j2));

            if (!mapa[i][j]) printf("*");
            printf("\t");
            //printf("%d", mapa[i][j]);
        }
    printf("\n");
    }



    return 0;
}