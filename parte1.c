#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define LONGITUD_MAX_LINEA 255

// se asume: el laberinto dado tiene solucion, y los puntos de inicio y fin no son '#'
int validar_archivo(char* nomArchivo) {

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
        case 0:
            check = sscanf(linea, "%d %d %d\n", &N, &M, &D);
            if (check != 3)
                archivoValido = 0;
            break;
        case 1:
            check = sscanf(linea, "%d %d\n", &i1, &j1);
            if (check != 2)
                archivoValido = 0;
            break;
        case 2:
            check = sscanf(linea, "%d %d\n", &i2, &j2);
            if (check != 2)
                archivoValido = 0;
            break;
        default:
            // leer laberinto
            char* l;
            unsigned int j; 
            // la cant filas del archivo discrepta con el nro de filas pasado
            //printf("k-2: %d, N: %d\n", k-2, N);
            if (k-2 > N)
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
        printf("Numero incorrecto de argumentos\n");
        return -1;
    }

    int v = validar_archivo(argv[1]);

    if (!v) {
        printf("El formato del archivo no es valido (chequear que no haya salto de linea al final)\n");
        return -1;
    }
    printf("El archivo es valido\n");

    return 0;
}