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
    // es la minima que podria tener, ya que 0 no se admite
    ir->distSensorConocida = 1;

    fprintf(stderr, "%d %d %d\n", ir->N, ir->M, distSensorMax);
    fprintf(stderr, "%d %d\n", ir->i1, ir->j1);
    fprintf(stderr, "%d %d\n", ir->i2, ir->j2);

    inicializa(ir);
    ComputeShortestPath(ir);
    
    int distancias[4], pasos = 0, movMax = 150;
    State siguiente, otro_posible;
    // ? probablemente lo saque
    otro_posible.nodo = (Nodo){-1, -1};
    State* posiblesSiguientes = malloc(sizeof(State) * 2);
    int cantPosibles = 0, a;

    // while s_start != s_goal
    while(ir->x != ir->i2 || ir->y != ir->j2) {
        fprintf(stderr, "robot: (%d, %d)\n", ir->x, ir->y);
        fprintf(stderr, "heap:\n"); bheap_recorrer(ir->cp, imprime_nodo);
    
        // podrian haber dos nodos con el mismo g-valor para que el robot se mueva
        cantPosibles = siguiente_movimiento(ir, posiblesSiguientes);
        
        fprintf(stderr, "heap:"); 
        bheap_recorrer(ir->cp, imprime_nodo);

        if (cantPosibles == 2) {
            if (ir->mapaInterno[posiblesSiguientes[0].nodo.x][posiblesSiguientes[0].nodo.y].tipoCasilla == VALIDO
            && ir->mapaInterno[posiblesSiguientes[1].nodo.x][posiblesSiguientes[1].nodo.y].tipoCasilla == VALIDO) {
                a = aleatorio();
                siguiente = posiblesSiguientes[a];
                otro_posible = posiblesSiguientes[!a];
            }
            else if (ir->mapaInterno[posiblesSiguientes[0].nodo.x][posiblesSiguientes[0].nodo.y].tipoCasilla 
        != VALIDO) {
                siguiente = posiblesSiguientes[1];
        } else {
                siguiente = posiblesSiguientes[0];
            }
        }
        else siguiente = posiblesSiguientes[0];

        fprintf(stderr, "finalmente se elige %d,%d\n", siguiente.nodo.x, siguiente.nodo.y);
    
       if (ir->mapaInterno[siguiente.nodo.x][siguiente.nodo.y].tipoCasilla == VALIDO ||
       ir->mapaInterno[siguiente.nodo.x][siguiente.nodo.y].tipoCasilla == VISITADO) {
            
            
            fprintf(stderr,"siguiente es valido\n");
            if (pasos >= movMax) {
                movMax *= 2;
                ir->rastro = realloc(ir->rastro, sizeof(char) * movMax);
            }
            pasos = mover_robot(ir, siguiente.nodo, pasos);  
            ir->mapaInterno[siguiente.nodo.x][siguiente.nodo.y].tipoCasilla = VISITADO;
        } 
        
        else if (ir->mapaInterno[siguiente.nodo.x][siguiente.nodo.y].tipoCasilla == DESCONOCIDO) {

            fprintf(stderr,"siguiente es desconocido\n");
            fprintf(stderr, "*Tirar sensor\n");
            printf("%c %d %d\n", '?', ir->x, ir->y);
            fflush(stdout);

            obtener_distancias(distancias, ir);
            actualizar_mapa_interno(ir, distancias, (cantPosibles == 2 && otro_posible.nodo.x != -1));
        }
        // evita loop infinito en caso de algun error
        if (strlen(ir->rastro) > 450) break; 
    }   
    ir->rastro[pasos] = '\0';

    // Mandar solucion al sensor para terminar la ejecucion
    printf("%c %s\n", '!', ir->rastro);
    fflush(stdout);
    fprintf(stderr, "Camino: %s\n", ir->rastro);
    
    // liberar memoria
    for (int i = 0; i < ir->N; i++) free(ir->mapaInterno[i]);
    free(posiblesSiguientes);
    free(ir->mapaInterno);
    bheap_destruir(ir->cp);
    free(ir->rastro);
    free(ir);
    return 0;
}