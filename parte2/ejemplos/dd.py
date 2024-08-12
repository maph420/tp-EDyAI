import numpy as np
import random

def generar_mapa(tamaño, probabilidad_obstaculo):
    mapa = np.full((tamaño, tamaño), '.')
    for i in range(tamaño):
        for j in range(tamaño):
            if random.random() < probabilidad_obstaculo:
                mapa[i][j] = '#'

    # Asignar coordenadas de inicio y fin aleatoriamente
    inicio = (random.randint(0, tamaño-1), random.randint(0, tamaño-1))
    fin = (random.randint(0, tamaño-1), random.randint(0, tamaño-1))
    while inicio == fin or mapa[inicio] == '#' or mapa[fin] == '#':
        inicio = (random.randint(0, tamaño-1), random.randint(0, tamaño-1))
        fin = (random.randint(0, tamaño-1), random.randint(0, tamaño-1))

    mapa[inicio] = 'S'  # Inicio
    mapa[fin] = 'F'  # Fin

    return mapa, inicio, fin


# Parámetros
tamano = 75
probabilidad_obstaculo = 0.19

# Generar mapa
mapa, inicio, fin = generar_mapa(tamano, probabilidad_obstaculo)

# Imprimir mapa
for fila in mapa:
    print(''.join(fila))

print("Coordenadas de inicio:", inicio)
print("Coordenadas de fin:", fin)