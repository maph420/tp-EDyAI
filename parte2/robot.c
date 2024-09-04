#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "robot_utils.h"

/**
 * Lee las distancias pasadas por el sensor y las guarda en el arreglo
 * d. Tambien actualiza la longitud del sensor de ser necesario.
 */
void obtener_distancias(int* d, InfoRobot* ir) {
    int m;
    if (scanf("%d %d %d %d", &d[0], &d[1], &d[2], &d[3]) != 4) {
        fprintf(stderr, "Error al leer las distancias del sensor\n");
        exit(EXIT_FAILURE);
    } 

    if ((m = max(d, 4)) > ir->distSensorConocida) {
        ir->distSensorConocida = m-1;
    } 
}

int main() {

    InfoRobot* ir = malloc(sizeof(InfoRobot));
    int distSensorMax;

    if (scanf("%d %d %d\n%d %d\n %d %d", &(ir->N), &(ir->M), &distSensorMax,
    &(ir->i1), &(ir->j1), &(ir->i2), &(ir->j2)) != 7) {
        fprintf(stderr, "Error al leer los argumentos del archivo\n");
        exit(EXIT_FAILURE) ;
    }
    
    InicializaRobot(ir);
    CalcularRutaOptima(ir);

    int distancias[4], pasos = 0, movMax = ir->N * ir->M;
    int numPosiblesMov = 0;
    EstadoConCoord posiblesSiguientes[2], sig;
    Coord ant;
    ant.x = ir->i1; ant.y = ir->i2;

    // Mientras el robot no alcance la meta
    while(ir->x != ir->i2 || ir->y != ir->j2) {
    
        numPosiblesMov = siguiente_movimiento(ir, posiblesSiguientes);
        
        if (numPosiblesMov == 2) {
            if (posiblesSiguientes[0].est.tipoCelda == VALIDO
            && posiblesSiguientes[1].est.tipoCelda == VALIDO) {
                sig = posiblesSiguientes[aleatorio()];
            }
            else if (posiblesSiguientes[0].est.tipoCelda != VALIDO) {
                sig = posiblesSiguientes[1];
        } else {
                sig = posiblesSiguientes[0];
            }
        }
        else sig = posiblesSiguientes[0]; 


       if (sig.est.tipoCelda == VALIDO || sig.est.tipoCelda == VISITADO) {
            
            if (pasos >= movMax) {
                movMax *= 2;
                ir->rastro = realloc(ir->rastro, sizeof(char) * movMax);
            }
            pasos = mover_robot(ir, sig.c, pasos);  
            ir->mapaInterno[sig.c.x][sig.c.y].tipoCelda = VISITADO;
        } 
        
        else if (sig.est.tipoCelda == DESCONOCIDO) {
            /**
             * El siguiente nodo de la ruta optima es desconocido,
             * tirar escaner para seguir la ruta, o de lo contrario
             * replanificar
             */
            printf("%c %d %d\n", '?', ir->x, ir->y);
            fflush(stdout);

            obtener_distancias(distancias, ir);
            actualizar_mapa_interno(ir, distancias, ant);
        }

    }   
    ir->rastro[pasos] = '\0';

    // Mandar solucion al sensor para terminar la ejecucion
    printf("%c %s\n", '!', ir->rastro);
    fflush(stdout);
    
    // Liberar memoria de estructuras usadas
    for (int i = 0; i < ir->N; i++) free(ir->mapaInterno[i]);
    free(ir->mapaInterno);
    bheap_destruir(ir->cp);
    free(ir->rastro);
    free(ir);
    return 0;
}