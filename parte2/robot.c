#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "estr/heap.h"
#include "robot_utils.h"

// TODO: ver como manejar cuando el sensor es 0, por ahora es comportamiento
// indefinido (?)

void obtener_distancias(int* d) {
    if (scanf("%d %d %d %d", &d[0], &d[1], &d[2], &d[3]) != 4) {
        fprintf(stderr, "Error al leer las distancias del sensor\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Distancias recibidas: Arriba=%d, Abajo=%d, Izquierda=%d, Derecha=%d\n", 
           d[0], d[1], d[2], d[3]);
}

int main(int argc, char** argv) {

    InfoRobot* ir = malloc(sizeof(InfoRobot));
    // leer datos del archivo (de sensor)
    if (scanf("%d %d %d\n%d %d\n %d %d", &(ir->N), &(ir->M), &(ir->d_max),
    &(ir->i1), &(ir->j1), &(ir->i2), &(ir->j2)) != 7) {
        fprintf(stderr, "Error al leer los argumentos del archivo\n");
        exit(EXIT_FAILURE) ;
    }

    fprintf(stderr, "%d %d %d\n", ir->N, ir->M, ir->d_max);
    fprintf(stderr, "%d %d\n", ir->i1, ir->j1);
    fprintf(stderr, "%d %d\n", ir->i2, ir->j2);

    inicializa(ir);
    Node ini = (Node){ir->i1, ir->j1};
    ComputeShortestPath(ir, ini);
    
    fprintf(stderr, "mapa:\n"); impr_mapa(ir);

    ir->x = ir->i1; ir->y = ir->j1;
    //ir->mapaInterno[ir->i1][ir->j1].tipoCasilla = VISITADO;
    int distancias[4], contAdyacentes = 0;
    State sig_est, *ady, tmp;
    State est_tmp;
    Node node_tmp = (Node){-1, -1};
    est_tmp.node = node_tmp;
    State* posiblesSiguientes = malloc(sizeof(State) * 2);
    int cantPosibles = 0, al;

    // while s_start != s_goal
    int c = 0, pasos = 0;
    while(ir->x != ir->i2 || ir->y != ir->j2) {
        fprintf(stderr, "robot: (%d, %d)\n", ir->x, ir->y);

        
        cantPosibles = siguiente_movimiento(ir, ir->x, ir->y, posiblesSiguientes);
        
        if (cantPosibles == 2 
        && ir->mapaInterno[posiblesSiguientes[0].node.x][posiblesSiguientes[0].node.y].tipoCasilla == VALIDO
        && ir->mapaInterno[posiblesSiguientes[1].node.x][posiblesSiguientes[1].node.y].tipoCasilla == VALIDO) {
            al = aleatorio();
            fprintf(stderr, "aleatorio: %d, otro: %d\n", al, !al);
            sig_est = posiblesSiguientes[al];
            est_tmp = posiblesSiguientes[!al];
        } else {
            sig_est = (ir->mapaInterno[posiblesSiguientes[0].node.x][posiblesSiguientes[0].node.y].tipoCasilla 
        != VALIDO) && cantPosibles == 2 ? posiblesSiguientes[1] : posiblesSiguientes[0];
        }

       if (ir->mapaInterno[sig_est.node.x][sig_est.node.y].tipoCasilla == VALIDO) {
            
            fprintf(stderr,"siguiente es valido\n");
            pasos = mover_robot(ir, sig_est.node, pasos);  
            //ir->mapaInterno[sig_est.node.x][sig_est.node.y].tipoCasilla = VISITADO;
        } 
        
        else if (ir->mapaInterno[sig_est.node.x][sig_est.node.y].tipoCasilla == DESCONOCIDO) {
            
            fprintf(stderr,"siguiente es desconocido\n");
            fprintf(stderr, "*Tirar sensor\n");
            printf("%c %d %d\n", '?', ir->x, ir->y);
            fflush(stdout);

            obtener_distancias(distancias);
            actualizar_mapa_interno(ir, distancias, sig_est, (cantPosibles == 2 && est_tmp.node.x != -1));

            fprintf(stderr, "---\nmapa ahora\n"); 
            impr_mapa(ir);
            fprintf(stderr, "---\n");       
        
            
        }

        if (c++ >= 90) break; 
    }   
    ir->rastro[pasos] = '\0';
    // Mandar solucion al sensor para terminar la ejecucionh|
    printf("%c %s\n", '!', ir->rastro);
    fflush(stdout);

    fprintf(stderr, "Camino: %s\n", ir->rastro);
    return 0;
}