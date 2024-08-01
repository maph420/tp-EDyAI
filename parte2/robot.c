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
    //fprintf(stderr, "Distancias recibidas: Arriba=%d, Abajo=%d, Izquierda=%d, Derecha=%d\n", 
      //     d[0], d[1], d[2], d[3]);
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
    ComputeShortestPath(ir);

    ir->x = ir->i1; ir->y = ir->j1;
    ir->mapaInterno[ir->i1][ir->j1].est = VISITADO;
    int distancias[4], contAdyacentes = 0;
    State sig_est, *ady;
    // while s_start != s_goal
    int c = 0, pasos = 0;
    while(ir->x != ir->i2 || ir->y != ir->j2) {
        fprintf(stderr, "robot: (%d, %d)\n", ir->x, ir->y);

        sig_est = siguiente_movimiento(ir, ir->x, ir->y);
        
        
        if (ir->mapaInterno[sig_est.node.x][sig_est.node.y].est == OBSTACULO) {
            // actualizar ruta
            fprintf(stderr, "actualizar %d, %d\n", ir->x, ir->y);
            //ir->mapaInterno[sig_est.node.x][sig_est.node.y].rhs = 100;
            UpdateVertex(sig_est, ir);
            contAdyacentes = 0;
            ady = obt_ady(ir, sig_est, &contAdyacentes);

            //fprintf(stderr, "contady: %d\n", contAdyacentes);
            //fprintf(stderr, "obt los ady\n");            
            //for (int h = 0; h < contAdyacentes; h++) fprintf(stderr, "%d %d\n", ady[h].node.x, ady[h].node.y);

            for (int h = 0; h < contAdyacentes; h++) 
                UpdateVertex(ir->mapaInterno[ady[h].node.x][ady[h].node.x], ir);

            ComputeShortestPath(ir);
            //fprintf(stderr, "mapa actualizado:\n"); impr_mapa(ir);
            // todo: hacer que se le pase a obt_ady un State* con
            // malloc ya asignado
            free(ady);
        }
        else if (ir->mapaInterno[sig_est.node.x][sig_est.node.y].est != DESCONOCIDO) {
            pasos = mover_robot(ir, sig_est.node, pasos);  
        } else {
            fprintf(stderr, "Tirar sensor\n");
            printf("%c %d %d\n", '?', ir->x, ir->y);
            fflush(stdout);

            obtener_distancias(distancias);
            actualizar_mapa_interno(ir, distancias);

            //fprintf(stderr, "---\nmapa ahora\n"); 
            //impr_mapa(ir);
            //fprintf(stderr, "---\n");
        }

        //getchar() ;
        //ir->x = ir->i2; ir->y = ir->j2;
        //printf("! LL\n");
        //if (c++ >= 24) break;
    }   
    ir->rastro[pasos] = '\0';
    // Mandar solucion al sensor para terminar la ejecucionh|
    printf("%c %s\n", '!', ir->rastro);
    fflush(stdout);

    fprintf(stderr, "Camino %s\n", ir->rastro);
    //ir->x = 0; ir->y = 4;
    //ir->mapaInterno[ir->y][ir->x].est = 15;
    //impr_mapa(ir);
    return 0;
}