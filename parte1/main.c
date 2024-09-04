#include "robot_utils.h"
#include <string.h>

/**
 * Valida que el archivo respete el formato pedido, guarda el mapa del laberinto, asi
 * como sus dimensiones y le pasa a la estructura de informacion del robot las coords de 
 * inicio y fin. Se asume: el laberinto dado tiene solucion, y los puntos de inicio y fin 
 * no son '#'  
 */
int validar_arch_y_guardar_info(char* nomArchivo, char*** mapa, unsigned int* nFilas, unsigned int* nCols, InfoRobot* ir) {

    char linea[LONGITUD_MAX_LINEA];
    FILE* archivoEntrada;
    unsigned int N, M, D, check, archivoValido = 1;
    int i1, i2, j1, j2;

    archivoEntrada = fopen(nomArchivo, "r");
    if (archivoEntrada == NULL) return 0;

    printf("Archivo '%s' abierto con exito\n", nomArchivo);
    for (unsigned int k = 0; (fgets(linea, LONGITUD_MAX_LINEA, archivoEntrada)) && archivoValido; k++) {
        switch (k) {
        case 0: {
            check = sscanf(linea, "%d %d %d\n", &N, &M, &D);
            if (check != 3) {
                archivoValido = 0;
            }
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
            check = sscanf(linea, "%d %d\n", &j1, &i1);
            if (check != 2) {
                archivoValido = 0;
            }
            else {
                ir->i1 = i1;
                ir->j1 = j1;
            }
            break;
        case 2: 
            check = sscanf(linea, "%d %d\n", &j2, &i2);
            if (check != 2) {
                archivoValido = 0;
            }
            else {
                ir->i2 = i2;
                ir->j2 = j2;
            }
            break;
        default:
            // leer laberinto
            char* l;
            unsigned int j; 
            // la cant filas del archivo es distinto a la cant de filas pasada
            if (k-3 > *nFilas)
                archivoValido = 0;
            else {
                if (k-2 > N || strlen(linea)-1 != M) { 
                    archivoValido = 0;
                }

                for (l = linea, j = 0; *l != '\n' && j < M && archivoValido; l++, j++) {
                    if (*l != '.' && *l != '#') {
                        archivoValido = 0;
                    }
                    else {
                        (*mapa)[k-3][j] = *l;
                    }
                }
            }
            break;
        }
    }
    fclose(archivoEntrada);
    return archivoValido ;
}

int main (int argc, char** argv) {

    if (argc != 2) {
        printf("Numero incorrecto de argumentos. Uso: ./parte1 nombre_archivo\n"); fflush(stdout);
        exit(EXIT_FAILURE);
    }

    char** mapa = NULL;
    unsigned int numFilas, numCols;
    InfoRobot infoRobot;
    // El numero de pasos maximos inicial. De ser necesario, es incrementado.
    int movMax = 150;
    Direccion* dirArr = malloc(sizeof(Direccion) * 5);

    int v = validar_arch_y_guardar_info(argv[1], &mapa, &numFilas, &numCols, &infoRobot);
    if (!v) {
        printf("El archivo no es valido (chequear formato)\n");
        exit(EXIT_FAILURE);
    }

    inicializa_robot(&infoRobot, movMax);
    movimiento_robot(&infoRobot, mapa, numFilas, numCols, movMax, dirArr);
    
    // Mostrar recorrido hecho por el robot
    for (int i = 0; infoRobot.rastro[i]; i++)
        printf("%c", infoRobot.rastro[i]);
    puts("");

    // liberar memoria usada por las estructuras
    for (unsigned int i = 0; i < numFilas; i++) free(mapa[i]);
    free(mapa);
    tablahash_destruir(infoRobot.visitados);
    pila_destruir(infoRobot.camino, nodomapa_destruir) ;
    free(infoRobot.rastro);
    free(dirArr);
    return 0;
}