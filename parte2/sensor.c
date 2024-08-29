#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#define LONGITUD_MAX_LINEA 1000
/*
e.g. aca el robot 'x' NO reconoceria al obstaculos 'o'
simplemente retorna derecha = 2 porque no sabe lo que hay

x . o

si fuera nolongitud del scanner 2, entonces:
x . o
seguiria dando derecha = 2, de donde aqui si podemos inferir
que hay obstaculo.

En general, si el scanner es de longitud k, las direcciones que
retornen k+1, es porque NO detectaron obstaculo, y es seguro 
moverse hasta ahi k veces en la pos pertinente desde la pos actual

*/

// leer_archivo()
// - lee el archivo `data` segun el formato especificado en el trabajo.
// - n, m, max_d, i1, j1, i2 y j2 son parametros de salida. Se corresponden con
//   los valores N, M, D, i1, j1, i2 y j2, respectivamente.
// - Devuelve un arreglo de arreglos con las filas del entorno, o NULL en caso
//   de que el formato sea invalido
// supone el archivo ya abierto, y es valido

char** leer_archivo(FILE* data, int* nFilas, int* nCols, int* max_d, int* i1, int* j1, int* i2, int* j2) {

    char linea[LONGITUD_MAX_LINEA], **mapa;
    unsigned int check, archivoValido = 1;

    if (data == NULL) return 0;

    //printf("Archivo '%s' abierto con exito\n", nomArchivo);
    for (int k = 0; (fgets(linea, LONGITUD_MAX_LINEA, data)) && archivoValido; k++) {
        //printf("k -> %d\n", k);
        switch (k) {
        case 0: {
            check = sscanf(linea, "%d %d %d\n", nFilas, nCols, max_d);
			/* Se considera que la longitud del sensor debe ser un numero
			natural positivo */
            if (check != 3 || *max_d <= 0) {
                archivoValido = 0;
			}
			
            else {
                mapa = malloc(sizeof(char*) * *nFilas);
                for (int h = 0; h < *nFilas; h++) 
                    mapa[h] = malloc(*nCols * sizeof(char));
            }
            break;
        }
        case 1:
            check = sscanf(linea, "%d %d\n", i1, j1);
            if (check != 2) {
                archivoValido = 0;
			}
            break;
        case 2: 
            check = sscanf(linea, "%d %d\n", i2, j2);
            if (check != 2) {
                archivoValido = 0;
				}
            break;
        default:
            // leer laberinto
            char* l; int j; 
            // la cant filas del archivo discrepta con el nro de filas pasado
            ////printf("k-2: %d, N: %d\n", k-2, N);
            if (k-3 > *nFilas)
                archivoValido = 0;
            else {

                ////printf("long linea: %ld, M: %d\n", strlen(linea), M);
				int largoLinea = strlen(linea);
                if (k-2 > *nFilas || largoLinea-1 != *nCols){ 
                    archivoValido = 0;
                }

                for (l = linea, j = 0; *l != '\n' && j < *nCols && archivoValido; l++, j++) {
                    if (*l != '.' && *l != '#')
                        archivoValido = 0;
                    else {
                        mapa[k-3][j] = *l;
                    }
                }
            }
            break;
        }
    }
    return archivoValido? mapa : NULL ;
}

// El resto de este archivo no tiene relevancia para el estudiante.

int coordenada_valida(int n, int m, int i, int j) {
	return 0 <= i && i < n && 0 <= j && j < m;
}

int lanzar_rayo(char** mapa, int n, int m, int i, int j, int di, int dj, int max_dist) {
	int dist = 0;
	// avanza en las direcciones di, dj pasadas hasta chocarse con un obstaculo
	// o querer salirse del mapa (coord invalida)
	while (coordenada_valida(n, m, i, j) && mapa[i][j] != '#') {
		dist++;
		i += di; j += dj;
	}
	// retorna donde se choco o donde se corta el rayo
	return dist > max_dist ? max_dist+1 : dist;
}

int correr_sensor(char** mapa, int n, int m, int i, int j, int* d, int max_d) {
	if (!coordenada_valida(n, m, i, j)) return 0;
	d[0] = lanzar_rayo(mapa, n, m, i, j, -1, 0, max_d); // arriba
	d[1] = lanzar_rayo(mapa, n, m, i, j,  1, 0, max_d); // abajo
	d[2] = lanzar_rayo(mapa, n, m, i, j, 0, -1, max_d); // izquierda
	d[3] = lanzar_rayo(mapa, n, m, i, j, 0,  1, max_d); // derecha
	return 1;
}

void caracter_inesperado(char c) {
	fprintf(stderr, "Comando invalido. Caracter inesperado: '%c' (ascii %d)\n", c, (int)c); 
	fflush(stderr);
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "Uso: ./sensor <archivo>\n"); fflush(stderr);
		return EXIT_FAILURE;
	}
	FILE* data = fopen(argv[1], "r");
	if (!data) {
		fprintf(stderr, "No existe el archivo\n"); fflush(stderr);
		return EXIT_FAILURE;
	}

	int n, m, max_d;
	int i1, j1;
	int i2, j2;
	char** mapa;
	if ((mapa = leer_archivo(data, &n, &m, &max_d, &i1, &j1, &i2, &j2)) == NULL) {
		fprintf(stderr, "Formato de archivo invalido\n");
		fclose(data);
		return EXIT_FAILURE;
	}

	fclose(data);

	fprintf(stderr, "Archivo leido\n");

	// todo: manejar para que primero lea esto
	printf("%d %d %d\n", n, m, max_d);
	printf("%d %d\n", i1, j1);
	printf("%d %d\n", i2, j2);
	fflush(stdout);

	int operaciones = 0;
	// se queda esperando comandos de robot
	while (1) {
		// toma caracter del stdin
		char c = getchar();

		if (isspace(c)) continue;
		else if (c == '?') {

			c = getchar();
			if (c != ' ')  {
				
			caracter_inesperado(c);
			}

			int i, j;
			if (scanf("%d%d", &i, &j) != 2) {
				caracter_inesperado(c);
			}
			operaciones++;
			int d[4] = {};
			correr_sensor(mapa, n, m, i, j, d, max_d);

			for (int i = 0; i < 4; ++i) {
				printf("%d%c", d[i], i == 3 ? '\n' : ' ');
			}
			fflush(stdout);

		} else if (c == '!') {
			
			c = getchar();
			if (c != ' ') caracter_inesperado(c);
			int distancia = 0;
			int i = i1, j = j1;
			while (1) {
				distancia++;
				c = getchar();
				 if (c == 'U') i -= 1;
				else if (c == 'D') i += 1;
				else if (c == 'L') j -= 1;
				else if (c == 'R') j += 1;
				else if (c == '\n') break;
				else caracter_inesperado(c);
			}

			if (i == i2 && j == j2) {
				fprintf(stderr, "El robot llego a su destino.\n");
				fprintf(stderr, "Operaciones realizadas: %d, Distancia recorrida: %d\n", operaciones, distancia);
				return EXIT_SUCCESS;
			} else {
				fprintf(stderr, "El robot no llego a su destino. Posicion final: (%d, %d).\n", i, j);
				return EXIT_FAILURE;
			}

		} else {
			fprintf(stderr, "aca ta\n");
			caracter_inesperado(c);
		}
	}
}