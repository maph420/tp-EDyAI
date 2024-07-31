#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "estr/heap.h"
#include "func.h"

int main(int argc, char** argv) {
    InfoRobot* ir = malloc(sizeof(InfoRobot));
    ir->N = 5; ir->M = 6; 
    ir->i1 = 0; ir->j1 = 0;
    ir->i2 = 5; ir->j2 = 4;
    inicializa(ir);

    //impr_mapa(ir);

    ComputeShortestPath(ir);

    return 0;
}