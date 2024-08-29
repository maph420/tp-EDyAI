#include "robot_utils.h"

unsigned int hash(void* dato) {
  int x = ((NodoMapa*)dato)->x;
  int y = ((NodoMapa*)dato)->y;
  return (y << 16) ^ x;
}

void* nodomapa_copia(void* n) {
    NodoMapa* copia = malloc(sizeof(NodoMapa)) ;
    copia->x = ((NodoMapa*)n)->x;
    copia->y = ((NodoMapa*)n)->y;
    copia->valido = ((NodoMapa*)n)->valido;
    return copia;
}

void nodomapa_destruir(void* n) {
    free((NodoMapa*)n) ;
}

int nodomapa_comparar(void* refA, void* refB) {
    NodoMapa* a = (NodoMapa*)refA;
    NodoMapa* b = (NodoMapa*)refB;

    return (a->x < b->x) ? -1 : 
    (a->x > b->x) ? 1 : 
    (a->y < b->y) ? -1 :
    (a->y > b->y) ? 1 : 0;
}

/*
static void imprimir_nodo(void* ref) {
    NodoMapa* nm = (NodoMapa*)ref;
    printf("(%d, %d)\tvalido: %d\n", nm->y, nm->x, nm->valido);
}*/

// sacar
char* print_dir(int d) { 
    if (d == 0) return "IZQ";
    else if (d == 1) return "DER";
    else if (d == 2) return "ARR";
    else if (d == 3) return "ABA";
    else return "INV";
}

unsigned int movimiento_valido(char** mapa, int N, int M, int posX, int posY) {
    return (posX >= M || posY >= N || posX < 0 || posY < 0) ? 0 : (mapa[posY][posX] == '.');
}

int aleatorio() {
    srand(time(NULL)) ;
    return (rand() % 2);
}

Direccion opuesta(Direccion d) {
    if (d == ABA) return ARR;
    if (d == ARR) return ABA;
    if (d == IZQ) return DER;
    if (d == DER) return IZQ;
    return d;
}

// toma estructura auxiliar de informacion de nodos, y aumenta la posicion actual 
// que tenga en 1, segun la direccion d pasada
int sig_nodo_x(Direccion d, int x) { 
    return d == IZQ ? --x : (d == DER) ? ++x : x;
}

int sig_nodo_y(Direccion d, int y) { 
    return d == ABA ? ++y : (d == ARR) ? --y : y;
}

// se asume que el robot se encuentra parado en una posicion valida
Direccion obtener_direccion(InfoRobot* ir, char** mapa, unsigned N, unsigned M, Direccion dOrigen) {
    NodoMapa v;
    // creo en cada vuelta esto
    Direccion d[4];
    // cuando estoy alineado con la meta en alguno de los ejes, solo va a haber
    // 1 direccion preferida (que sea elejible, es otro tema)
    int c = 2, r = aleatorio();
    d[r] = (ir->y == ir->j2) ? INV : (ir->y < ir->j2) ? ABA : ARR;
    d[!r] = (ir->x == ir->i2) ? INV : (ir->x < ir->i2) ? DER : IZQ;
    // reconocer la otra direccion, la que no es candidata, pero tampoco es la menos deseable
    for (int h = 0; h < 4; h++) {
        if ((Direccion)h != d[0] && (Direccion)h != d[1] && (Direccion)h != opuesta(dOrigen)) {
            d[c++] = (Direccion)h;
        }
    }  

    for (int l = 0; l < c; l++) {
        //printf("Prueba direccion: %s\n", print_dir(d[l]));
        v = (NodoMapa){
            sig_nodo_x(d[l], ir->x),
            sig_nodo_y(d[l], ir->y),
            1,
            d[l]};
        if (d[l] != INV && movimiento_valido(mapa, N, M, v.x, v.y) && 
        !tablahash_buscar(ir->visitados, &v)) return d[l];
        //else printf("no se pudo, probar con otra\n");
    }
    // ninguna direccion parece factible => backtrackear
    //printf("Ninguna direccion parece valida, backtrackear\n");
    return INV;
}

// se asume: la direccion reciba no es INV
char asignar_direccion(Direccion d) {
    return d == IZQ ? 'L' : d == DER ? 'R' : d == ABA ? 'D' : 'U';
}

void movimiento_robot(InfoRobot* ir, char** mapa, unsigned int N, unsigned int M) {

    printf("(%d, %d) -> (%d, %d)\n", ir->i1, ir->j1, ir->i2, ir->j2);
    unsigned int pasos = 0, movimientosMax = 50;
    ir->camino = pila_crear();
    int tamInicialTabla = primo_mas_cercano(abs(ir->i2 - ir->i1) * abs(ir->j2 - ir->j1) * 1.8);
    printf("tamanio: %d\n", tamInicialTabla);
    ir->visitados = tablahash_crear(tamInicialTabla, nodomapa_copia, nodomapa_comparar, nodomapa_destruir, hash);
    ir->rastro = malloc(sizeof(char) * movimientosMax) ;

    NodoMapa* b = malloc(sizeof(NodoMapa));
    *b = (NodoMapa){ir->i1, ir->j1, 1, INV};
    pila_apilar((&ir->camino), b, nodomapa_copia);

    ir->x = ir->i1; ir->y = ir->j1;
    Direccion dirActual = INV;

    while ((ir->x != ir->i2) || (ir->y != ir->j2)) {

        //printf("Robot: (%d, %d)\n", ir->y, ir->x);
        
        // indica al robot el proximo movimiento
       // printf("Calcular direccion\n");
       
        dirActual = obtener_direccion(ir, mapa, N, M, dirActual);
        int alcanzoMeta = 0, porChocarse = 0;

        /* mientras el robot pueda moverse en una misma direccion que lo acerque a la
        meta, lo hace */
        while (!alcanzoMeta && dirActual != INV && !porChocarse) { 
            
            //printf("Robot: (%d, %d)\n", ir->y, ir->x);
            //printf("movimiento -> %s\n", print_dir(dirActual));
            //getchar();

            b->x = sig_nodo_x(dirActual, ir->x);
            b->y = sig_nodo_y(dirActual, ir->y);
            
            if (movimiento_valido(mapa, N, M, b->x, b->y)) {
                
                if (pasos >= movimientosMax) {
                    movimientosMax *= 2;
                    ir->rastro = realloc(ir->rastro, sizeof(char) * movimientosMax);
                }
                ir->rastro[pasos++] = asignar_direccion(dirActual);
                
                // mover el robot a la sig pos
                ir->x = b->x; ir->y = b->y; b->dirOrigen = dirActual; b->valido = 1;
                tablahash_insertar(ir->visitados, b, FACTOR_CARGA_UMBRAL);
                pila_apilar(&(ir->camino), b, nodomapa_copia);
            } 
            else { 
                porChocarse = 1;
                //printf("Cambia de movimiento\n");
                //getchar();
                b->valido = 0; b->dirOrigen = dirActual;
                tablahash_insertar(ir->visitados, b, FACTOR_CARGA_UMBRAL);
            }

            /* el robot se movio hasta estar alineado en algun eje con la meta,
            dejar de ir en la misma direccion, por si hace falta recalcular la
            direccion que esta tomando */
       
            if (ir->x == ir->i2 && ir->y == ir->j2)
                alcanzoMeta = 1;

            else if (ir->x == ir->i2 || ir->y == ir->j2) {
                //printf("alineado con alguno de los ejes\n");
                dirActual = obtener_direccion(ir, mapa, N, M, dirActual);
                //printf("probar ahora: %s\n", print_dir(dirActual));
            } 
            
        }
        //printf("sale del while, alcanzometa=%d, porchocarse:%d, diractual: %s\n",
        //alcanzoMeta, porChocarse, print_dir(dirActual));
        //porChocarse = 0;
         // vamos al anterior
        if (dirActual == INV) {
            
            //printf("Pila: \n\n");
            //pila_imprimir(ir->camino, imprimir_nodo);
            //printf("BACKTRACK: retroceder un lugar\n");

            pila_desapilar(&(ir->camino));
            NodoMapa* ant = (NodoMapa*)pila_tope(ir->camino);
            // chequear
            Direccion dirVuelta = (ir->y == ant->y) ? (ant->x < ir->x) ? IZQ : DER : 
            (ant->y < ir->y) ? ARR : ABA; 

            //printf("nodo anterior: %d, %d, nodo actual: %d, %d\n",
            //ant->y, ant->x, ir->y, ir->x);

            //printf("dir vuelta: %s\n", print_dir(dirVuelta)) ;

            // se mueve "hacia atras" 1 lugar
            ir->x = ant->x; ir->y = ant->y;

            if (pasos >= movimientosMax) {
                    movimientosMax *= 2;
                    ir->rastro = realloc(ir->rastro, sizeof(char) * movimientosMax);
                }
            ir->rastro[pasos++] = asignar_direccion(dirVuelta);
        }
        }
        free(b);
        ir->rastro[pasos] = '\0' ;
        }