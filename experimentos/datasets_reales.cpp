// datasets_reales.cpp
// Pregunta 2.3
// Floyd-Warshall sobre bio-SC-TS.edges y power-685-bus.mtx.
//
// Uso:   g++ -std=c++17 -O2 -o datasets_reales.exe datasets_reales.cpp
//        ./datasets_reales.exe
// Salida: resultados/bio-SC-TS_resultado.csv, resultados/power-685-bus_resultado.csv

#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "../src/apsp.hpp"
#include "../src/grafo.hpp"
#include "../src/parser_dataset.hpp"

// ---------- Utilidad ----------
// Escribe la matriz D en formato CSV: nodo1,nodo2,distancia.
void escribir_csv(const Matriz& D, const std::string& ruta) {
    std::ofstream salida(ruta);
    if (!salida) {
        std::fprintf(stderr, "Error: no se pudo crear %s\n", ruta.c_str());
        return;
    }
    salida << "nodo1,nodo2,distancia\n";
    const int n = static_cast<int>(D.size());
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            salida << i << "," << j << ",";
            if (std::isinf(D[i][j]))
                salida << "INF";
            else
                salida << D[i][j];
            salida << "\n";
        }
    }
    salida.close();
    std::printf("  CSV escrito: %s\n", ruta.c_str());
}

// ---------- Programa principal ----------

int main() {
    using Clock = std::chrono::high_resolution_clock;
    std::printf("---- Pregunta 2.3: Floyd-Warshall sobre datasets reales ----\n\n");

    // ---------------------------------------------------------------
    // Dataset 1: bio-SC-TS.edges
    // Fuente: https://networkrepository.com/bio-SC-TS.php
    // Autor:  Ryan A. Rossi & Nesreen K. Ahmed (Network Repository, AAAI 2015)
    // ---------------------------------------------------------------
    {
        std::printf("--- bio-SC-TS.edges ---\n");
        std::printf("  Cargando...\n");
        Grafo g = leer_edges("../bio-SC-TS.edges");
        std::printf("  n = %d, m = %zu\n", g.n, g.aristas.size());

        std::printf("  Ejecutando Floyd-Warshall...\n");
        bool ciclo = false;
        auto t0 = Clock::now();
        Matriz D = floyd_warshall(g, &ciclo);
        auto t1 = Clock::now();

        auto duracion = std::chrono::duration<double>(t1 - t0).count();
        std::printf("  Tiempo: %.6f s\n", duracion);
        std::printf("  Ciclo negativo: %s\n", ciclo ? "SI" : "NO");

        escribir_csv(D, "resultados/bio-SC-TS_resultado.csv");
        std::printf("\n");
    }

    // ---------------------------------------------------------------
    // Dataset 2: power-685-bus.mtx
    // Fuente: https://networkrepository.com/power-685-bus.php
    // Autor:  Ryan A. Rossi & Nesreen K. Ahmed (Network Repository, AAAI 2015)
    // ---------------------------------------------------------------
    {
        std::printf("--- power-685-bus.mtx ---\n");
        std::printf("  Cargando...\n");
        Grafo g = leer_mtx("../power-685-bus.mtx");
        std::printf("  n = %d, m = %zu (incluye aristas simetricas)\n",
                    g.n, g.aristas.size());

        std::printf("  Ejecutando Floyd-Warshall...\n");
        bool ciclo = false;
        auto t0 = Clock::now();
        Matriz D = floyd_warshall(g, &ciclo);
        auto t1 = Clock::now();

        auto duracion = std::chrono::duration<double>(t1 - t0).count();
        std::printf("  Tiempo: %.6f s\n", duracion);
        std::printf("  Ciclo negativo: %s\n", ciclo ? "SI" : "NO");

        escribir_csv(D, "resultados/power-685-bus_resultado.csv");
        std::printf("\n");
    }

    std::printf("----- Pregunta 2.3 completada -----\n");
    return 0;
}
