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

static void imprimir_nodo(void* ref) {
    NodoMapa* nm = (NodoMapa*)ref;
    printf("(%d, %d)\tvalido: %d\n", nm->y, nm->x, nm->valido);
}


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

    printf("Archivo '%s' abierto con exito\n", nomArchivo);
    for (unsigned int k = 0; (fgets(linea, LONGITUD_MAX_LINEA, archivoEntrada)) && archivoValido; k++) {
        // printf("k -> %d\n", k);
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
            //printf("k-2: %d, N: %d\n", k-2, N);
            if (k-3 > N)
                archivoValido = 0;
            else {
                //printf("long linea: %ld, M: %d\n", strlen(linea), M);

                if (!(k-2 < N && strlen(linea)-1 == M))
                    archivoValido = 0;
                if (k-2 == N && strlen(linea) == M)
                    archivoValido = 1;
                //if ( k == M && ! (strlen(linea) == M))
                  //  archivoValido = 0;
                
                for (l = linea, j = 0; *l != '\n' && j < M && archivoValido; l++, j++) {
                    //printf("leido: %c\t", *l);
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
    else return "ABA";
}

unsigned int movimiento_valido(int** mapa, int N, int M, int posX, int posY) {

    if (posX >= M || posY >= N || posX < 0 || posY < 0) {
        printf("%d %d no es valido\n", posY, posX);
        return 0;
    }
    if (mapa[posY][posX]) {
        printf("%d %d es valido\n", posY, posX);
        return 1;
    }
    else {
        printf("%d %d no es valido\n", posY, posX);
        return 0;
    } 
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
void sig_nodo(Direccion d, NodoMapa* nm) {
    switch (d)
            {
            case ABA:
                nm->y++;
                break;
            case ARR:
                nm->y--;
                break;
            case IZQ:
                nm->x--;
                break;
            case DER:
                nm->x++;
                break;
            case INV:
                printf("BACKTRACK\n");
                break;
        }
}
// Toma la estructura con la información del robot y lo desplaza 1 unidad en
// la direccion d pasada.
void moverse(Direccion d, InfoRobot* ir) {
    switch (d)
            {
            case ABA:
                ir->y++;
                break;
            case ARR:
                ir->y--;
                break;
            case IZQ:
                ir->x--;
                break;
            case DER:
                ir->x++;
                break;
            case INV:
                printf("BACKTRACK\n");
                break;
        }
}
// se asume que el robot se encuentra parado en una posicion valida, asimismo que 
// la meta es valida
Direccion obtener_direccion(InfoRobot* ir, int** mapa, unsigned N, unsigned M, Direccion dOrigen, NodoMapa* verificador) {
    
    Direccion dir1 = (ir->y == ir->j2) ? INV : (ir->y < ir->j2) ? ABA : ARR;
    Direccion dir2 = (ir->x == ir->i2) ? INV : (ir->x < ir->i2) ? DER : IZQ;

    // Verificar las direcciones preferidas (dir1 y dir2)
    printf("Preferidas: %s, %s\n", print_dir(dir1), print_dir(dir2));
    Direccion pref = aleatorio()? dir1 : dir2;
    Direccion segPref = (pref == dir1)? dir2 : dir1;
    //printf("Probar primer alternativa: %s\n", print_dir(pref));

    verificador->x = ir->x; verificador->y = ir->y;
    sig_nodo(pref, verificador);
    if (pref != INV && movimiento_valido(mapa, N, M, verificador->x, verificador->y) 
    && !avl_buscar(ir->visitados, verificador)) return pref;

    // si llegamos aca, la dir preferida no se puede, probamos la segunda preferida
    // (si la hay)
    if (pref != INV)
        sig_nodo(opuesta(pref), verificador);
    
    sig_nodo(segPref, verificador);
    //printf("Probar segunda alternativa\n");
    if (segPref != INV && movimiento_valido(mapa, N, M, verificador->x, verificador->y) 
    && !avl_buscar(ir->visitados, verificador)) return segPref;

     // no nos podemos mover en ninguna de las direcciones preferidas, veamos
     // si podemos movernos en alguna otra que no sea la op de donde venimos
    if (segPref != INV)
        sig_nodo(opuesta(segPref), verificador);
    
    for (int i = 0 ; i < 4; i++) {
        if ((Direccion)i != pref && (Direccion)i != segPref && (Direccion)i != opuesta(dOrigen)) {
            if ((Direccion)i != INV) {
                //printf("De ultima, se intenta: %s\n", print_dir((Direccion)i));
                //printf("Pos actual: %d, %d\n", verificador->y, verificador->x);
                sig_nodo((Direccion)i, verificador);
                if (i != INV && movimiento_valido(mapa, N, M, verificador->x, 
                verificador->y) && !avl_buscar(ir->visitados, verificador)) return i;
            }
        }
    }
    // si tampoco se puede, la unica opcion sera backtracker, retornar invalido
    return INV;
}

// se asume: la direccion pasada no es invalida
char asignar_direccion(Direccion d) {
    if (d == IZQ) return 'L';
    if (d == DER) return 'R';
    if (d == ABA) return 'D';
    return 'U';
}

int movimiento_robot(InfoRobot* ir, int** mapa, unsigned int N, unsigned int M) {

    printf("(%d, %d) -> (%d, %d)\n", ir->i1, ir->j1, ir->i2, ir->j2);
    printf("MAPA:\n");
    for (unsigned int i=0; i < N; i++) {
        for(unsigned int j = 0; j < M; j++)
            printf("%d", mapa[i][j]);
    printf("\n");
    }

    NodoMapa* v = malloc(sizeof(NodoMapa)) ;
    unsigned int pasos = 0, movimientosMax = 50;
    ir->camino = pila_crear();
    ir->visitados = avl_crear(nodomapa_copia, nodomapa_comparar, nodomapa_destruir);
    ir->rastro = malloc(sizeof(char) * movimientosMax) ;

    // la pos actual del robot es la inicial
    NodoMapa* nm = malloc(sizeof(NodoMapa));
    NodoMapa* b = malloc(sizeof(NodoMapa));
    ir->x = ir->i1; ir->y = ir->j1;
    b->x = ir->x; b->y = ir->y;
    Direccion dirActual = INV;

    while ((ir->x != ir->i2) || (ir->y != ir->j2)) {
        dirActual = obtener_direccion(ir, mapa, N, M, dirActual, v);
        printf("proximo movimiento (es valido): %s\n", print_dir(dirActual));
        int flag = 1, porChocarse = 0;

        while (flag && dirActual != INV && !porChocarse) {
            printf("Robot: (%d, %d)\n", ir->y, ir->x);
            //printf("movimiento -> %s\n", print_dir(dirActual));
            
            nm->x = ir->x; nm->y = ir->y;
            nm->valido = 1;
            nm->dirOrigen = dirActual;

            b->x = ir->x; b->y = ir->y;
            sig_nodo(dirActual, b);
            if (movimiento_valido(mapa, N, M, b->x, b->y)) {
                pila_apilar(&(ir->camino), nm, nodomapa_copia);
                avl_insertar(ir->visitados, nm);
                if (pasos >= movimientosMax) {
                    movimientosMax *= 2;
                    ir->rastro = realloc(ir->rastro, sizeof(char) * movimientosMax);
                }
                ir->rastro[pasos++] = asignar_direccion(dirActual);
                moverse(dirActual, ir);
            } 
            else porChocarse = 1;

            if (ir->x == ir->i2 || ir->y == ir->j2) {
                printf("y: %d, x:%d\n", ir->y, ir->x) ;
                printf("i2: %d, j2: %d\n", ir->i2, ir->j2);
                flag = 0;
            }
            //pila_imprimir(ir->camino, imprimir_nodo);
            printf("AVL\n");
            avl_recorrer(ir->visitados, AVL_RECORRIDO_IN, imprimir_nodo);
            getchar();
        }
        /*
        if (dirActual != INV && flag) {
            printf("mueve en la opuesta \n");
            moverse(opuesta(dirActual), ir) ;
            printf("Robot: (%d, %d)\n", ir->y, ir->x);
            ir->rastro[pasos++] = asignar_direccion(opuesta(dirActual));
        }*/
       printf("porChocarse: %d\n", porChocarse) ;
       if (porChocarse) {
            NodoMapa* invalido = malloc(sizeof(NodoMapa));
            *invalido = (NodoMapa){b->x, b->y, 0, dirActual};
            
            avl_insertar(ir->visitados, invalido) ;
            printf("Cambia de movimiento\n");
            getchar();
       }
            
        flag = 1 ;
        porChocarse = 0;

         // vamos al anterior
        if (dirActual == INV) {
            printf("BACKTRACK: retroceder un lugar\n");
            NodoMapa* ant = (NodoMapa*)pila_tope(ir->camino);
            // se mueve "hacia atras" 1 lugar
            //printf("(%d %d), (%d, %d)\n", ir->y, ir->x, ant->y, ant->x);

            Direccion dirVuelta = (ir->y == ant->y) ? (ant->x < ir->x) ? IZQ : DER : 
            (ant->y < ir->y) ? ARR : ABA; 

            ir->x = ant->x;
            ir->y = ant->y;
            //printf("dir vuelta: %s\n", print_dir(dirVuelta));
            ir->rastro[pasos++] = asignar_direccion(dirVuelta);
            pila_desapilar(&(ir->camino));
        }
       
        }

        free(v);
        ir->rastro[pasos] = '\0' ;
        pila_imprimir(ir->camino, imprimir_nodo) ;
        return 1;
        }


int main (int argc, char** argv) {

    // validacion argumentos
    if (argc != 2) {
        printf("Numero incorrecto de argumentos\n");
        return -1;
    }

    // variables para guardar info del archivo
    int** mapa = NULL;
    unsigned int numFilas, numCols;
    InfoRobot* infoRobot = malloc(sizeof(InfoRobot));
    infoRobot->f = movimiento_valido;

    int v = validar_arch_y_guardar_info(argv[1], &mapa, &numFilas, &numCols, infoRobot);

    if (!v) {
        printf("El formato del archivo no es valido (chequear que no haya salto de linea al final)\n");
        return -1;
    }
    printf("El archivo es valido\n");

    int r;

    r = movimiento_robot(infoRobot, mapa, numFilas, numCols);

    printf("Movimiento robot -> %d\n", r) ;

    printf("Recorrido hecho:\n");
    for (int i = 0; infoRobot->rastro[i]; i++)
        printf("%c", infoRobot->rastro[i]);
    puts("");
    printf("Pasos: %zu\n", strlen(infoRobot->rastro));


    // Liberar memoria usada
    avl_destruir(infoRobot->visitados);
    pila_destruir(infoRobot->camino, nodomapa_destruir) ;
    free(infoRobot->rastro);
    free(infoRobot) ;

    return 0;
}