#include <stdio.h>

int main() {
    // Enviar comando de consulta de sensor
    fflush(stdout);
    printf("%c 0 3\n", 63);
    fflush(stdout); // Asegúrate de que el comando se envíe inmediatamente

    printf("!");
    // Leer la respuesta del sensor
    int distancias[4];
    if (scanf("%d %d %d %d", &distancias[0], &distancias[1], &distancias[2], &distancias[3]) != 4) {
        fprintf(stderr, "Error al leer las distancias del sensor\n");
        return 1;
    }

    //printf("sdjfjlkas\n");
    // Imprimir las distancias recibidas
    fprintf(stderr, "Distancias recibidas: Arriba=%d, Abajo=%d, Izquierda=%d, Derecha=%d\n", 
           distancias[0], distancias[1], distancias[2], distancias[3]);


    // Enviar comando de movimiento
    printf("! U U U R R R D D D\n");
    fflush(stdout);

    return 0;
}