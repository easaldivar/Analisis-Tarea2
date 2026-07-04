# Analisis-Tarea2

Tarea 2 — 501404/503309 Análisis de Algoritmos (2026-2).

**Integrantes:** Erick Saldivar — Alejandro Neira.

Problema: *Distancia Mínima entre todo par de nodos* (APSP). Se implementan y
comparan el **Algoritmo Base** (n ejecuciones de Bellman-Ford) y el
**Algoritmo de Floyd-Warshall** (programación dinámica, Θ(n³)).

## Estructura del repositorio

| Ruta | Contenido |
|---|---|
| `informe/` | Informe en LaTeX (`informe.tex`) |
| `src/grafo.hpp` | Representación del grafo dirigido con pesos y lectura desde archivo |
| `src/apsp.hpp` | `bellman_ford`, `algoritmo_base` y `floyd_warshall` |
| `src/verificar.cpp` | Verificación de correctitud (pregunta 2.1) |
| `tests/casos/` | Instancias con solución conocida (`*.grafo` + `*.esperado`) |
| `uhr-main/` | Suite de medición sugerida en el enunciado |
| `Chebyshev2.mtx`, `bio-SC-TS.edges`, `power-685-bus.mtx` | Datasets reales (pregunta 2.3) |

## Requisitos

- `g++` con soporte C++17 (probado con GCC 16.1.0, MinGW-w64/MSYS2 en Windows 11).
- Para el informe: una distribución LaTeX con `pdflatex` (o Overleaf).

## Verificación de correctitud (pregunta 2.1)

Desde la **raíz del repositorio**:

```
g++ -std=c++17 -O2 -o verificar src/verificar.cpp
./verificar
```

El programa ejecuta dos baterías (descritas en la sección 2 del informe):

1. **Instancias con solución conocida** (`tests/casos/`): ambos algoritmos
   deben reproducir exactamente la matriz esperada, o detectar el ciclo
   negativo cuando corresponde.
2. **Verificación cruzada** sobre 200 grafos aleatorios reproducibles
   (semilla fija): ambos algoritmos deben coincidir entrada a entrada.

Imprime `[OK]`/`[FALLA]` por prueba y termina con código de salida 0 solo si
todas pasan. Puede indicarse otro directorio de casos como primer argumento:
`./verificar tests/casos`.

## Formato de instancias (`tests/casos/*.grafo`)

```
n m
u v w        (m líneas: arista dirigida u -> v con peso real w; nodos 0-indexados)
```

El archivo `*.esperado` asociado contiene la matriz n×n de distancias
(el token `INF` denota "no existe camino"), o la palabra `CICLO_NEGATIVO` si
la instancia contiene un ciclo de peso negativo.

## Compilar el informe

```
cd informe
pdflatex informe.tex
pdflatex informe.tex     # segunda pasada para referencias cruzadas
```

(El informe incluye los códigos fuente como anexo vía rutas relativas, por lo
que debe compilarse desde el directorio `informe/`.)
