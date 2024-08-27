#include "robot_utils.h"

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
int sig_nodo_x(Direccion d, int valX) {
    switch (d)
            {
            case IZQ:
                return --valX;
            case DER:
                return ++valX;
            default:
                return valX;
        }
}

int sig_nodo_y(Direccion d, int valY) {
     switch (d)
            {
            case ABA:
                return ++valY;
                break;
            case ARR:
                return --valY;
            default: 
                return valY;
    }
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
        v = (NodoMapa){
            sig_nodo_x(d[l], ir->x),
            sig_nodo_y(d[l], ir->y),
            1,
            d[l]};
        if (d[l] != INV && movimiento_valido(mapa, N, M, v.x, v.y) && 
        !avl_buscar(ir->visitados, &v)) return d[l];
    }
    // ninguna direccion parece factible => backtrackear
    return INV;
}

// se asume: la direccion reciba no es INV
char asignar_direccion(Direccion d) {
    return d == IZQ ? 'L' : d == DER ? 'R' : d == ABA ? 'D' : 'U';
}

void movimiento_robot(InfoRobot* ir, char** mapa, unsigned int N, unsigned int M) {

    unsigned int pasos = 0, movimientosMax = 50;
    ir->camino = pila_crear();
    ir->visitados = avl_crear(nodomapa_copia, nodomapa_comparar, nodomapa_destruir);
    ir->rastro = malloc(sizeof(char) * movimientosMax) ;

    NodoMapa* b = malloc(sizeof(NodoMapa));
    *b = (NodoMapa){ir->i1, ir->j1, 1, INV};
    pila_apilar((&ir->camino), b, nodomapa_copia);

    ir->x = ir->i1; ir->y = ir->j1;
    Direccion dirActual = INV;

    while ((ir->x != ir->i2) || (ir->y != ir->j2)) {
        
        // indica al robot el proximo movimiento
        dirActual = obtener_direccion(ir, mapa, N, M, dirActual);
        int flag = 1, porChocarse = 0;

        while (flag && dirActual != INV && !porChocarse) { 
            
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
                avl_insertar(ir->visitados, b);
                pila_apilar(&(ir->camino), b, nodomapa_copia);
            } 
            else { 
                porChocarse = 1;
                b->valido = 0; b->dirOrigen = dirActual;
                avl_insertar(ir->visitados, b) ;
            }

            if (ir->x == ir->i2 || ir->y == ir->j2) {
                //printf("alineado con alguno de los ejes\n");
                flag = 0;
            }
            printf("sale del while\n");
        }

        flag = 1 ;
        porChocarse = 0;
         // vamos al anterior
        if (dirActual == INV) {
            
            pila_desapilar(&(ir->camino));
            NodoMapa* ant = (NodoMapa*)pila_tope(ir->camino);
            // chequear
            Direccion dirVuelta = (ir->y == ant->y) ? (ant->x < ir->x) ? IZQ : DER : 
            (ant->y < ir->y) ? ARR : ABA; 

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