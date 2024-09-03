#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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
    Estado sig, posiblesSiguientes[2], last;
    last.coord.x = ir->i1; last.coord.y = ir->i2;

    int cantPosibles = 0;

    // while s_start != s_goal
    while(ir->x != ir->i2 || ir->y != ir->j2) {
        //fprintf(stderr, "robot: (%d, %d)\n", ir->x, ir->y);
    
        // podrian haber dos nodos con el mismo valor "ideal" para que el robot 
        // se mueva
        cantPosibles = siguiente_movimiento(ir, posiblesSiguientes);
        
        if (cantPosibles == 2) {
            if (posiblesSiguientes[0].tipoCelda == VALIDO
            && posiblesSiguientes[1].tipoCelda == VALIDO) {
                sig = posiblesSiguientes[aleatorio()];
            }
            else if (posiblesSiguientes[0].tipoCelda != VALIDO) {
                sig = posiblesSiguientes[1];
        } else {
                sig = posiblesSiguientes[0];
            }
        }
        else sig = posiblesSiguientes[0]; 

       // fprintf(stderr, "finalmente se elige %d,%d\n", sig.pos.x, sig.pos.y);
    
       if (sig.tipoCelda == VALIDO || sig.tipoCelda == VISITADO) {
            
           // fprintf(stderr,"siguiente es valido\n");
            if (pasos >= movMax) {
                movMax *= 2;
                ir->rastro = realloc(ir->rastro, sizeof(char) * movMax);
            }
            pasos = mover_robot(ir, sig.coord, pasos);  
            ir->mapaInterno[sig.coord.x][sig.coord.y].tipoCelda = VISITADO;
        } 
        
        else if (sig.tipoCelda == DESCONOCIDO) {

            //fprintf(stderr,"siguiente es de onocido\n");
            //fprintf(stderr, "*Tirar sensor\n");
            printf("%c %d %d\n", '?', ir->x, ir->y);
            fflush(stdout);

            obtener_distancias(distancias, ir);
            actualizar_mapa_interno(ir, distancias, last);
        }

    }   
    ir->rastro[pasos] = '\0';

    // Mandar solucion al sensor para terminar la ejecucion
    printf("%c %s\n", '!', ir->rastro);
    fflush(stdout);
    //fprintf(stderr, "Camino: %s\n", ir->rastro);
    
    // liberar memoria
    for (int i = 0; i < ir->N; i++) free(ir->mapaInterno[i]);
    free(ir->mapaInterno);
    bheap_destruir(ir->cp);
    free(ir->rastro);
    free(ir);
    return 0;
}