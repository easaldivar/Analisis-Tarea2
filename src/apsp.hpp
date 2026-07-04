#pragma once
// apsp.hpp -- Algoritmos para el problema de Distancia Minima entre todo par
// de nodos (APSP):
//   * Algoritmo Base: n ejecuciones de Bellman-Ford (pregunta 1.1).
//   * Algoritmo de Floyd-Warshall: programacion dinamica (pregunta 1.2).
// El diseno, correctitud y analisis de complejidad estan en el informe
// (secciones 1.1 y 1.2); la documentacion de la implementacion, en la
// seccion 2 del informe.

#include <limits>
#include <vector>

#include "grafo.hpp"

// Distancia "infinita": representa que no existe camino. La aritmetica IEEE
// de double garantiza INF + c == INF (para c finito) e INF + INF == INF, y
// toda comparacion "INF + c < x" con x finito es falsa; por lo tanto las
// relajaciones son seguras sin tratar casos especiales.
const double INF = std::numeric_limits<double>::infinity();

// Matriz de distancias: D[i][j] = distancia minima desde i hasta j.
using Matriz = std::vector<std::vector<double>>;

// ---------------------------------------------------------------------------
// Bellman-Ford (una fuente).
// Calcula dist[v] = distancia minima desde s hasta v para todo v, en O(n*m):
// a lo mas n-1 rondas en las que se intenta relajar cada arista. Si en una
// ronda ninguna distancia mejora, el algoritmo ya alcanzo un punto fijo y
// termina antes (esto no afecta la correctitud, solo el tiempo). Una ronda
// adicional detecta ciclos negativos alcanzables desde s: si alguna arista
// aun puede relajarse tras n-1 rondas, existe uno.
// Si ciclo_negativo != nullptr, se escribe alli el resultado de la deteccion.
// ---------------------------------------------------------------------------
inline std::vector<double> bellman_ford(const Grafo& g, int s,
                                        bool* ciclo_negativo = nullptr) {
    std::vector<double> dist(g.n, INF);
    dist[s] = 0.0;
    for (int ronda = 1; ronda <= g.n - 1; ++ronda) {
        bool mejoro = false;
        for (const Arista& a : g.aristas) {
            if (dist[a.u] + a.w < dist[a.v]) {
                dist[a.v] = dist[a.u] + a.w;
                mejoro = true;
            }
        }
        if (!mejoro) break;  // punto fijo alcanzado: terminar antes
    }
    if (ciclo_negativo != nullptr) {
        *ciclo_negativo = false;
        for (const Arista& a : g.aristas) {
            if (dist[a.u] + a.w < dist[a.v]) {
                *ciclo_negativo = true;
                break;
            }
        }
    }
    return dist;
}

// ---------------------------------------------------------------------------
// Algoritmo Base (pregunta 1.1).
// Resuelve APSP con exactamente n llamadas a Bellman-Ford, una por cada
// fuente: la fila s de la matriz resultado es la salida de bellman_ford(g,s).
// Complejidad: O(n^2 * m) en tiempo; Theta(n^2) en espacio (la salida).
// Si ciclo_negativo != nullptr, se escribe true si alguna llamada detecto un
// ciclo negativo (todo ciclo negativo es alcanzable desde sus propios nodos,
// de modo que al recorrer todas las fuentes ninguno pasa inadvertido).
// ---------------------------------------------------------------------------
inline Matriz algoritmo_base(const Grafo& g, bool* ciclo_negativo = nullptr) {
    if (ciclo_negativo != nullptr) *ciclo_negativo = false;
    Matriz D(g.n);
    for (int s = 0; s < g.n; ++s) {
        bool cn = false;
        D[s] = bellman_ford(g, s, &cn);
        if (cn && ciclo_negativo != nullptr) *ciclo_negativo = true;
    }
    return D;
}

// ---------------------------------------------------------------------------
// Algoritmo de Floyd-Warshall (pregunta 1.2).
// Programacion dinamica sobre el conjunto de vertices intermedios permitidos:
//   D_k[i][j] = min( D_{k-1}[i][j] , D_{k-1}[i][k] + D_{k-1}[k][j] ),
// con D_0 la matriz de pesos directos. La actualizacion se realiza in situ
// sobre una unica matriz: en la iteracion k la fila k y la columna k no
// cambian (Lema demostrado en la seccion 1.2 del informe), por lo que leer
// entradas ya actualizadas equivale a leer las de la etapa k-1.
// Complejidad: Theta(n^3) en tiempo y Theta(n^2) en espacio.
// Si ciclo_negativo != nullptr, se escribe true si existe un ciclo negativo,
// lo que ocurre si y solo si alguna entrada diagonal termina negativa.
// ---------------------------------------------------------------------------
inline Matriz floyd_warshall(const Grafo& g, bool* ciclo_negativo = nullptr) {
    // Caso base D_0: 0 en la diagonal, peso de la arista directa si existe
    // (si hay aristas paralelas se conserva la de menor peso), INF si no.
    Matriz D(g.n, std::vector<double>(g.n, INF));
    for (int i = 0; i < g.n; ++i) D[i][i] = 0.0;
    for (const Arista& a : g.aristas) {
        if (a.w < D[a.u][a.v]) D[a.u][a.v] = a.w;
    }

    for (int k = 0; k < g.n; ++k) {
        for (int i = 0; i < g.n; ++i) {
            const double dik = D[i][k];  // invariante durante la iteracion k
            for (int j = 0; j < g.n; ++j) {
                if (dik + D[k][j] < D[i][j]) D[i][j] = dik + D[k][j];
            }
        }
    }

    if (ciclo_negativo != nullptr) {
        *ciclo_negativo = false;
        for (int i = 0; i < g.n; ++i) {
            if (D[i][i] < 0) {
                *ciclo_negativo = true;
                break;
            }
        }
    }
    return D;
}
