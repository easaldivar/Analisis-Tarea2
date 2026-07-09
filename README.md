# Analisis-Tarea2

Tarea 2 - 501404/503309 Analisis de Algoritmos (2026-2).

**Integrantes:** Erick Saldivar - Alejandro Neira.

Problema: *Distancia Mínima entre todo par de nodos* (APSP). Se implementan y
comparan el **Algoritmo Base** (n ejecuciones de Bellman-Ford) y el
**Algoritmo de Floyd-Warshall** (programación dinámica, Θ(n³)).

## Estructura del repositorio

| Ruta | Contenido |
|---|---|
| `informe/` | Informe en LaTeX (`informe.tex`) |
| `src/grafo.hpp` | Representación del grafo dirigido con pesos y lectura desde archivo |
| `src/apsp.hpp` | `bellman_ford`, `algoritmo_base` y `floyd_warshall` |
| `src/parser_dataset.hpp` | Lectura de datasets en formato `.mtx` y `.edges` (pregunta 2.3) |
| `src/verificar.cpp` | Verificación de correctitud (pregunta 2.1) |
| `tests/casos/` | Instancias con solución conocida (`*.grafo` + `*.esperado`) |
| `uhr-main/` | Suite de medición sugerida en el enunciado |
| `experimentos/` | Experimentos de medición (preguntas 2.2 y 2.3) |
| `Chebyshev2.mtx`, `bio-SC-TS.edges`, `power-685-bus.mtx` | Datasets reales (pregunta 2.3) |

## Requisitos

- `g++` con soporte C++17 (probado con GCC 16.1.0, MinGW-w64/MSYS2 en Windows 11).
- Para los gráficos: Python 3 con `pandas`, `numpy` y `matplotlib`.
- Para el informe: una distribución LaTeX con `pdflatex` (o Overleaf).

## Verificación de correctitud (pregunta 2.1)

Desde la **raíz del repositorio**:

```
g++ -std=c++17 -O2 -o verificar src/verificar.cpp
./verificar
```

El programa ejecuta dos baterías (descritas en la sección 2.1 del informe):

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

## Experimentos de medición de tiempo (pregunta 2.2)

Cuatro experimentos comparan `algoritmo_base` y `floyd_warshall` variando
la estructura del grafo. Cada experimento genera dos CSVs (uno por algoritmo)
en `experimentos/resultados/` con columnas `n,t_mean,t_stdev,t_Q0,…,t_Q4`
(tiempos en microsegundos). Los ejecutables se compilan con **`-O2`**. Las llamadas a los algoritmos se
protegen con `volatile` para evitar que el compilador las elimine del bucle
de medición, incluso con optimizaciones habilitadas.

### Compilación

Desde `experimentos/`:

```bash
g++ -std=c++17 -O2 -o exp1_n_creciente.exe       exp1_n_creciente.cpp
g++ -std=c++17 -O2 -o exp2_densidad.exe           exp2_densidad.cpp
g++ -std=c++17 -O2 -o exp3_pesos_negativos.exe    exp3_pesos_negativos.cpp
g++ -std=c++17 -O2 -o exp4_grafos_completos.exe   exp4_grafos_completos.cpp
```

### Ejecución

Cada experimento recibe `<runs> <lower> <upper> <step>` (sin nombre de archivo
de salida; los CSVs tienen nombres fijos). Desde `experimentos/`:

```bash
# E1 - n creciente, densidad fija p = 0.3, pesos en [0, 10]
./exp1_n_creciente.exe 64 10 300 10
#   -> resultados/base_e1_us.csv + resultados/fw_e1_us.csv

# E2 - densidad creciente (p = 0.1 … 1.0), n = 100 fijo
./exp2_densidad.exe 64 10 100 10
#   -> resultados/base_e2_us.csv + resultados/fw_e2_us.csv

# E3 - n creciente, pesos en [-2, 10], p = 0.15
./exp3_pesos_negativos.exe 32 10 200 20
#   -> resultados/base_e3_us.csv + resultados/fw_e3_us.csv

# E4 - grafos completos (p = 1.0), pesos en [0, 10]
./exp4_grafos_completos.exe 32 10 200 20
#   -> resultados/base_e4_us.csv + resultados/fw_e4_us.csv
```

> **Nota:** `<runs>` debe ser ≥ 32. Para E4 se recomienda 32 porque el
> algoritmo base sobre grafos completos escala como $O(n^4)$.

### Gráficos

Con los CSVs generados, desde `experimentos/`:

```bash
pip install pandas numpy matplotlib   # solo la primera vez
python graficar.py
```

Produce 4 gráficos (PDF + PNG) en `resultados/graficos/` con puntos medidos,
barras de error y curvas de ajuste teórico ($R^2$ incluido).

---

## Datasets reales (pregunta 2.3)

Ejecuta **Floyd-Warshall** sobre los datasets `bio-SC-TS.edges` y
`power-685-bus.mtx`, reportando tiempo de ejecución y escribiendo la matriz
de distancias en formato CSV (`nodo1,nodo2,distancia`).

### Compilación

Desde `experimentos/`:

```bash
g++ -std=c++17 -O2 -o datasets_reales.exe datasets_reales.cpp
```

### Ejecución

Desde `experimentos/`:

```bash
./datasets_reales.exe
```

### Salida

| Archivo | Contenido |
|---|---|
| `resultados/bio-SC-TS_resultado.csv` | Matriz $636 \times 636$ (~5 MB) |
| `resultados/power-685-bus_resultado.csv` | Matriz $685 \times 685$ (~10 MB) |

> **Nota:** `power-685-bus.mtx` es una matriz de admitancia con pesos
> negativos que producen ciclos negativos; Floyd-Warshall lo detecta y las
> distancias en el CSV no son finitas.

---

## Compilar el informe

```
cd informe
pdflatex informe.tex
pdflatex informe.tex     # segunda pasada para referencias cruzadas
```

(El informe incluye los códigos fuente como anexo vía rutas relativas, por lo
que debe compilarse desde el directorio `informe/`.)
