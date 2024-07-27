#include "func.h"

#define LONGITUD_MAX_LINEA 255

void* nodomapa_copia(void* n) {
    NodoMapa* copia = malloc(sizeof(NodoMapa)) ;
    copia->x = ((NodoMapa*)n)->x;
    copia->y = ((NodoMapa*)n)->y;
    copia->valido = ((NodoMapa*)n)->valido;
    return copia;
}

//(?)
void nodomapa_destruir(void* n) {
    free((NodoMapa*)n) ;
}

int nodomapa_comparar(void* refA, void* refB) {
    NodoMapa* a = (NodoMapa*)refA;
    NodoMapa* b = (NodoMapa*)refB;

    if (a->x < b->x || ( (a->x == b->x) && (a->y < b->y) )) return -1;
    if (a->x > b->x || ( (a->x == b->x) && (a->y > b->y) )) return 1;
    return 0;
}

/*
static void imprimir_nodo(void* ref) {
    NodoMapa* nm = (NodoMapa*)ref;
    printf("(%d, %d)\tvalido: %d\n", nm->y, nm->x, nm->valido);
}*/

/**
 * Valida que el archivo respete el formato pedido, guarda el mapa del laberinto, asi
 * como sus dimensiones y le pasa a la estructura de info del robot las coords de inicio y fin
 *  */
// se asume: el laberinto dado tiene solucion, y los puntos de inicio y fin no son '#'
int validar_arch_y_guardar_info(char* nomArchivo, int*** mapa, unsigned int* nFilas, unsigned int* nCols, InfoRobot* ir) {

    char linea[LONGITUD_MAX_LINEA];
    FILE* archivoEntrada;
    unsigned int N, M, D, check, archivoValido = 1;
    int i1, i2, j1, j2;

    archivoEntrada = fopen(nomArchivo, "r");
    if (archivoEntrada == NULL) return 0;

    //printf("Archivo '%s' abierto con exito\n", nomArchivo);
    for (unsigned int k = 0; (fgets(linea, LONGITUD_MAX_LINEA, archivoEntrada)) && archivoValido; k++) {
        //printf("k -> %d\n", k);
        switch (k) {
        case 0: {
            check = sscanf(linea, "%d %d %d\n", &N, &M, &D);
            if (check != 3)
                archivoValido = 0;
            else {
                *nFilas = N;
                *nCols = M;
                *mapa = malloc(sizeof(int*) * N);
                for (unsigned int h = 0; h < N; h++) 
                    (*mapa)[h] = malloc(M * sizeof(int));
            }
            break;
        }
        case 1:
            check = sscanf(linea, "%d %d\n", &j1, &i1);
            if (check != 2)
                archivoValido = 0;
            else {
                ir->i1 = i1;
                ir->j1 = j1;
            }
            break;
        case 2: 
            check = sscanf(linea, "%d %d\n", &j2, &i2);
            if (check != 2)
                archivoValido = 0;
            else {
                ir->i2 = i2;
                ir->j2 = j2;
            }
            break;
        default:
            // leer laberinto
            char* l;
            unsigned int j; 
            // la cant filas del archivo discrepta con el nro de filas pasado
            ////printf("k-2: %d, N: %d\n", k-2, N);
            if (k-3 > N)
                archivoValido = 0;
            else {
                ////printf("long linea: %ld, M: %d\n", strlen(linea), M);

                if (!(k-2 < N && strlen(linea)-1 == M))
                    archivoValido = 0;
                if (k-2 == N && strlen(linea) == M)
                    archivoValido = 1;
                //if ( k == M && ! (strlen(linea) == M))
                  //  archivoValido = 0;
                
                for (l = linea, j = 0; *l != '\n' && j < M && archivoValido; l++, j++) {
                    ////printf("leido: %c\t", *l);
                    if (*l != '.' && *l != '#')
                        archivoValido = 0;
                    else {
                        (*mapa)[k-3][j] = (*l == '.');
                    }
                }
            }
            break;
        }
    }
    fclose(archivoEntrada);
    return archivoValido ;
}

char* print_dir(int d) {
    if (d == 0) return "IZQ";
    else if (d == 1) return "DER";
    else if (d == 2) return "ARR";
    else if (d == 3) return "ABA";
    else return "INV";
}

unsigned int movimiento_valido(int** mapa, int N, int M, int posX, int posY) {
    int d = (posX >= M || posY >= N || posX < 0 || posY < 0) ? 0 : mapa[posY][posX];
    //printf("%d, %d %s\n", posY, posX, d? "es valida" : "no es valida");
    return d;
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
            default: //(INV)
                return valX;
                break;
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
            default: //(INV)
                return valY;
    }
}
// se asume que el robot se encuentra parado en una posicion valida, asimismo que 
// la meta es valida
Direccion obtener_direccion(InfoRobot* ir, int** mapa, unsigned N, unsigned M, Direccion dOrigen) {
    NodoMapa v;
    
    // cuando estoy alineado con la meta en alguno de los ejes, solo va a haber
    // 1 direccion preferida (que sea elejible, es otro tema)
    Direccion dir1 = (ir->y == ir->j2) ? INV : (ir->y < ir->j2) ? ABA : ARR;
    Direccion dir2 = (ir->x == ir->i2) ? INV : (ir->x < ir->i2) ? DER : IZQ;

    // Verificar las direcciones preferidas (dir1 y dir2)
    //printf("Preferidas: %s, %s\n", print_dir(dir1), print_dir(dir2));
    Direccion pref = aleatorio()? dir1 : dir2;
    Direccion segPref = (pref == dir1)? dir2 : dir1;
    //printf("Probar primer alternativa: %s\n", print_dir(pref));

    v = (NodoMapa){sig_nodo_x(pref, ir->x), sig_nodo_y(pref, ir->y), 1, pref}; 

    if (pref != INV && movimiento_valido(mapa, N, M, v.x, v.y) 
    && !avl_buscar(ir->visitados, &v)) return pref;

    // si llegamos aca, la dir preferida no se puede, probamos la segunda preferida
    // (si la hay)
    //printf("Probar segunda alternativa\n");
    v = (NodoMapa){sig_nodo_x(segPref, ir->x), sig_nodo_y(segPref, ir->y), 1, segPref}; 
    if (segPref != INV && movimiento_valido(mapa, N, M, v.x, v.y) 
    && !avl_buscar(ir->visitados, &v)) return segPref;

     // no nos podemos mover en ninguna de las direcciones preferidas, veamos
     // si podemos movernos en alguna otra que no sea la op de donde venimos
    
    for (int i = 0 ; i < 4; i++) {
        if ((Direccion)i != pref && (Direccion)i != segPref && (Direccion)i != opuesta(dOrigen)) {
            if ((Direccion)i != INV) {
                //printf("De ultima, se intenta: %s\n", print_dir((Direccion)i));
                v = (NodoMapa){sig_nodo_x(i, ir->x), sig_nodo_y(i, ir->y), 1, i}; 
                if (i != INV && movimiento_valido(mapa, 
                N, M, v.x, v.y) && 
                !avl_buscar(ir->visitados, &v)) return i;
                //verificador->x = ir->x; verificador->y = ir->y;
            }
        }
    }
    // si tampoco se puede, la unica opcion sera backtracker, retornar invalido
    return INV;
}

// se asume: la direccion reciba no es INV
char asignar_direccion(Direccion d) {
    return d == IZQ ? 'L' : d == DER ? 'R' : d == ABA ? 'D' : 'U';
}

void movimiento_robot(InfoRobot* ir, int** mapa, unsigned int N, unsigned int M) {

    unsigned int recalculos = 0;
    unsigned int pasos = 0, movimientosMax = 50;
    ir->camino = pila_crear();
    ir->visitados = avl_crear(nodomapa_copia, nodomapa_comparar, nodomapa_destruir);
    ir->rastro = malloc(sizeof(char) * movimientosMax) ;

    // la pos actual del robot es la inicial
    NodoMapa nm = (NodoMapa){ir->i1, ir->j1, 1, INV};
    pila_apilar((&ir->camino), &nm, nodomapa_copia);

    NodoMapa* b = malloc(sizeof(NodoMapa));
    ir->x = ir->i1; ir->y = ir->j1;
    b->x = ir->x; b->y = ir->y;
    Direccion dirActual = INV;
    //, anterior;
    while ((ir->x != ir->i2) || (ir->y != ir->j2)) {
        //printf("Robot: (%d, %d)\n", ir->y, ir->x);
        dirActual = obtener_direccion(ir, mapa, N, M, dirActual);
        int flag = 1, porChocarse = 0;

        while (flag && dirActual != INV && !porChocarse) {
            //printf("Robot: (%d, %d)\n", ir->y, ir->x);
            //printf("movimiento -> %s\n", print_dir(dirActual));
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
                nm = (NodoMapa){ir->x, ir->y, 1, dirActual};
                avl_insertar(ir->visitados, &nm);
                pila_apilar(&(ir->camino), &nm, nodomapa_copia);
            } 
            else { porChocarse = 1; //choques++;
            }

            if (ir->x == ir->i2 || ir->y == ir->j2) {
                //printf("y: %d, x:%d\n", ir->y, ir->x) ;
                //printf("i2: %d, j2: %d\n", ir->i2, ir->j2);
                flag = 0;
            }
            //getchar();
        }
       //printf("porChocarse: %d\n", porChocarse) ;
       // guardar obstaculos
       if (porChocarse) {
            recalculos++;
            b->valido = 0; b->dirOrigen = dirActual;
            avl_insertar(ir->visitados, b) ;
            //printf("Cambia de movimiento\n");
            //getchar();
       }
        flag = 1 ;
        porChocarse = 0;
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

            //printf("nodo anterior: %d, %d, nodo actual: %d, %d\n",ant->y, ant->x, ir->y, ir->x);

            //printf("dir vuelta: %s\n", print_dir(dirVuelta)) ;
            // se mueve "hacia atras" 1 lugar
            ir->x = ant->x;
            ir->y = ant->y;
            if (pasos >= movimientosMax) {
                    //printf("pasos: %d\n", pasos);
                    movimientosMax *= 2;
                    ir->rastro = realloc(ir->rastro, sizeof(char) * movimientosMax);
                }
            ir->rastro[pasos++] = asignar_direccion(dirVuelta);
            
        }
        }

        //printf("recalculos: %d\n", recalculos);
        free(b);
        ir->rastro[pasos] = '\0' ;
        //pila_imprimir(ir->camino, imprimir_nodo) ;
        }


int main (int argc, char** argv) {

    // validacion argumentos
    if (argc != 2) {
        printf("Numero incorrecto de argumentos\n");
        exit(EXIT_FAILURE);
    }

    // variables para guardar info del archivo
    int** mapa = NULL;
    unsigned int numFilas, numCols;
    InfoRobot* infoRobot = malloc(sizeof(InfoRobot));
    infoRobot->f = movimiento_valido;

    int v = validar_arch_y_guardar_info(argv[1], &mapa, &numFilas, &numCols, infoRobot);
    if (!v) {
        printf("El formato del archivo no es valido (chequear que no haya salto de linea al final)\n");
        free(infoRobot);
        exit(EXIT_FAILURE);
    }

    movimiento_robot(infoRobot, mapa, numFilas, numCols);
    ////printf("Movimiento robot -> %d\n", r) ;
    ////printf("Recorrido hecho:\n");
    for (int i = 0; infoRobot->rastro[i]; i++)
        printf("%c", infoRobot->rastro[i]);
    puts("");
   
    //printf("%zu\n", strlen(infoRobot->rastro));
    fflush(stdout);
    // liberar memoria usada por las estructuras usadas
    for (unsigned int i = 0; i < numFilas; i++) free(mapa[i]);
    free(mapa);
    avl_destruir(infoRobot->visitados);
    pila_destruir(infoRobot->camino, nodomapa_destruir) ;
    free(infoRobot->rastro);
    free(infoRobot) ;
    return 0;
}