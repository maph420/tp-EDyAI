#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "robot_utils.h"

void obtener_distancias(int* d, InfoRobot* ir) {
    int m;
    if (scanf("%d %d %d %d", &d[0], &d[1], &d[2], &d[3]) != 4) {
        fprintf(stderr, "Error al leer las distancias del sensor\n");
        exit(EXIT_FAILURE);
    } 
    fprintf(stderr, "Distancias recibidas: Arriba=%d, Abajo=%d, Izquierda=%d, Derecha=%d\n", 
           d[0], d[1], d[2], d[3]);

    if ((m = max(d, 4))> ir->distSensorConocida) {
        ir->distSensorConocida = m-1;
    } 
}

int main() {

    InfoRobot* ir = malloc(sizeof(InfoRobot));
    int distSensorMax;
    // leer datos del archivo (de sensor)
    // se asume: el robot sabe las dimensiones del mapa
    // el robot no sabe: la dist maxima del sensor
    if (scanf("%d %d %d\n%d %d\n %d %d", &(ir->N), &(ir->M), &distSensorMax,
    &(ir->i1), &(ir->j1), &(ir->i2), &(ir->j2)) != 7) {
        fprintf(stderr, "Error al leer los argumentos del archivo\n");
        exit(EXIT_FAILURE) ;
    }

    fprintf(stderr, "%d %d %d\n", ir->N, ir->M, distSensorMax);
    fprintf(stderr, "%d %d\n", ir->i1, ir->j1);
    fprintf(stderr, "%d %d\n", ir->i2, ir->j2);

    inicializa(ir);
    ComputeShortestPath(ir);
    
    int distancias[4], pasos = 0, movMax = 150;
    Estado sig, posiblesSiguientes[2];
    int cantPosibles = 0;

    // while s_start != s_goal
    while(ir->x != ir->i2 || ir->y != ir->j2) {
        fprintf(stderr, "robot: (%d, %d)\n", ir->x, ir->y);
    
        // podrian haber dos nodos con el mismo valor "ideal" para que el robot 
        // se mueva
        cantPosibles = siguiente_movimiento(ir, posiblesSiguientes);
        
        //fprintf(stderr, "heap:\n"); 
        //bheap_recorrer(ir->cp, imprime_nodo);

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
  

        fprintf(stderr, "finalmente se elige %d,%d\n", sig.pos.x, sig.pos.y);
    
       if (sig.tipoCelda == VALIDO || sig.tipoCelda == VISITADO) {
            
            fprintf(stderr,"siguiente es valido\n");
            if (pasos >= movMax) {
                movMax *= 2;
                ir->rastro = realloc(ir->rastro, sizeof(char) * movMax);
            }
            pasos = mover_robot(ir, sig.pos, pasos);  
            ir->mapaInterno[sig.pos.x][sig.pos.y].tipoCelda = VISITADO;
        } 
        
        else if (sig.tipoCelda == DESCONOCIDO) {

            fprintf(stderr,"siguiente es desconocido\n");
            fprintf(stderr, "*Tirar sensor\n");
            printf("%c %d %d\n", '?', ir->x, ir->y);
            fflush(stdout);

            obtener_distancias(distancias, ir);
            actualizar_mapa_interno(ir, distancias);
        }
        // evita loop infinito en caso de algun error
        if (strlen(ir->rastro) > 400) break; 
    }   
    ir->rastro[pasos] = '\0';

    // Mandar solucion al sensor para terminar la ejecucion
    printf("%c %s\n", '!', ir->rastro);
    fflush(stdout);
    fprintf(stderr, "Camino: %s\n", ir->rastro);
    
    // liberar memoria
    for (int i = 0; i < ir->N; i++) free(ir->mapaInterno[i]);
    free(ir->mapaInterno);
    bheap_destruir(ir->cp);
    free(ir->rastro);
    free(ir);
    return 0;
}