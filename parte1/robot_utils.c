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

unsigned int movimiento_valido(char** mapa, int N, int M, int posX, int posY) {
    return (posX >= M || posY >= N || posX < 0 || posY < 0) ? 0 : (mapa[posY][posX] == '.');
}

int aleatorio() {
    srand(time(NULL)) ;
    return (rand() % 2);
}

void inicializa_robot(InfoRobot* ir, int movimientosMaxInicial) {
    // Coloca al robot en el inicio del mapa.
    ir->x = ir->i1; ir->y = ir->j1;
    ir->camino = pila_crear();
    int tamInicialTabla = primo_mas_cercano(abs(ir->i2 - ir->i1) * abs(ir->j2 - ir->j1) * 2);
    ir->visitados = tablahash_crear(tamInicialTabla, nodomapa_copia, nodomapa_comparar, nodomapa_destruir, hash);
    ir->rastro = malloc(sizeof(char) * movimientosMaxInicial) ;
}

int sig_nodo_x(Direccion d, int x) { 
    return d == IZQ ? --x : (d == DER) ? ++x : x;
}

int sig_nodo_y(Direccion d, int y) { 
    return d == ABA ? ++y : (d == ARR) ? --y : y;
}

Direccion opuesta(Direccion d) {
    switch(d) {
        case ABA: return ARR;
        case ARR: return ABA;
        case IZQ: return DER;
        case DER: return IZQ;
        default: return d;
    }
}

Direccion obtener_direccion(InfoRobot* ir, char** mapa, unsigned N, unsigned M, Direccion dOrigen, Direccion* d) {
    NodoMapa v;
    int c = 2, r = aleatorio();
    d[r] = (ir->y == ir->j2) ? INV : (ir->y < ir->j2) ? ABA : ARR;
    d[!r] = (ir->x == ir->i2) ? INV : (ir->x < ir->i2) ? DER : IZQ;
    // Reconocer la otra direccion, la que no es candidata, pero tampoco es la menos deseable
    for (Direccion h = IZQ; h <= ABA; h++) {
        if (h != d[0] && h != d[1] && h != opuesta(dOrigen)) {
            d[c++] = h;
        }
    }  

    /* Entre las direcciones candidatas, preferir las
    que aun no fueron visitadas */
    for (int l = 0; l < c; l++) {
        v = (NodoMapa){
            sig_nodo_x(d[l], ir->x),
            sig_nodo_y(d[l], ir->y)
            };
        if (movimiento_valido(mapa, N, M, v.x, v.y) && 
        !tablahash_buscar(ir->visitados, &v)) return d[l];
    }
    // Ninguna direccion parece factible => Backtrackear
    return INV;
}

char asignar_direccion(Direccion d) {
    return d == IZQ ? 'L' : d == DER ? 'R' : d == ABA ? 'D' : 'U';
}

void movimiento_robot(InfoRobot* ir, char** mapa, unsigned N, unsigned M, unsigned movMax, Direccion* dirArr) {

    unsigned int pasos = 0;
    Direccion dirActual = INV;
    
    // Estructura auxiliar con la que se verifica posiciones validas,
    // utilizada para guardar la informacion recopilada por el robot
    NodoMapa b = (NodoMapa){ir->i1, ir->j1};
    pila_apilar((&ir->camino), &b, nodomapa_copia);

    /* Mientras el robot no llegue a la meta */
    while ((ir->x != ir->i2) || (ir->y != ir->j2)) {

        // Indica el proximo movimiento
        dirActual = obtener_direccion(ir, mapa, N, M, dirActual, dirArr);

        b.x = sig_nodo_x(dirActual, ir->x);
        b.y = sig_nodo_y(dirActual, ir->y);
            
        if (dirActual != INV) {
                
            if (pasos >= movMax) {
                movMax *= 2;
                ir->rastro = realloc(ir->rastro, sizeof(char) * movMax);
            }
            ir->rastro[pasos++] = asignar_direccion(dirActual);
              
            // Mover el robot a la sig posicion
            ir->x = b.x; ir->y = b.y; 
            tablahash_insertar(ir->visitados, &b, FACTOR_CARGA_UMBRAL);
            pila_apilar(&(ir->camino), &b, nodomapa_copia);
        }
        // El movimento es invalido => (obstaculo / limite de mapa) 
        else {
            pila_desapilar(&(ir->camino), nodomapa_destruir);
            NodoMapa* ant = (NodoMapa*)pila_tope(ir->camino);
            // Se mueve "hacia atras" 1 lugar
            ir->x = ant->x; ir->y = ant->y;

            Direccion dirVuelta = (ir->y == ant->y) ? (ant->x < ir->x) ? IZQ : DER : 
            (ant->y < ir->y) ? ARR : ABA; 

            if (pasos >= movMax) {
                movMax *= 2;
                ir->rastro = realloc(ir->rastro, sizeof(char) * movMax);
            }
            ir->rastro[pasos++] = asignar_direccion(dirVuelta);
        }
    }
    ir->rastro[pasos] = '\0';
}