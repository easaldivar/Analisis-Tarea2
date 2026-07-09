// exp3_pesos_negativos.cpp
// Experimento 3 (Pregunta 2.2)
// n creciente, densidad fija p = 0.15, pesos en [-2, 10].
//
// Uso:   ./exp3 <runs> <lower> <upper> <step>
// Ej:    ./exp3 32 10 200 20
// Salida: resultados/base_e3_us.csv, resultados/fw_e3_us.csv

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../src/apsp.hpp"
#include "../src/grafo.hpp"
#include "experimento_utils.hpp"

int main(int argc, char *argv[])
{
    std::int64_t runs, lower, upper, step;
    validate_input(argc, argv, runs, lower, upper, step);

    // --- Parametros del experimento ---
    const double DENSIDAD = 0.15;   // p = 0.15: balance dispersion/rendimiento
    const double PESO_MIN = -2.0;   // negativos para probar sensibilidad de BF
    const double PESO_MAX = 10.0;
    const unsigned SEMILLA = 20260706u; // fija para reproducibilidad

    std::mt19937_64 rng(SEMILLA);
    std::uniform_real_distribution<double> peso_dist(PESO_MIN, PESO_MAX);

    // Archivos de salida
    std::ofstream csv_base("resultados/base_e3_us.csv");
    std::ofstream csv_fw("resultados/fw_e3_us.csv");
    auto escribir_encabezado = [](std::ofstream &f)
    {
        f << "n,t_mean,t_stdev,t_Q0,t_Q1,t_Q2,t_Q3,t_Q4\n";
    };
    escribir_encabezado(csv_base);
    escribir_encabezado(csv_fw);

    const std::int64_t total_pruebas = ((upper - lower) / step + 1);
    const std::int64_t total_runs = runs * total_pruebas;

    std::vector<double> times(runs);
    std::vector<double> q;
    double mean_time, time_stdev, dev;
    using Clock = std::chrono::high_resolution_clock;
    using Dur = std::chrono::duration<double, std::micro>;

    std::cerr << "\033[0;36mExperimento 3: pesos negativos, p=" << DENSIDAD
              << ", pesos=[" << PESO_MIN << "," << PESO_MAX
              << "] (n=" << lower << ".." << upper << ", step=" << step
              << ", runs=" << runs << ")\033[0m\n\n";

    std::int64_t ejecutadas = 0;
    for (std::int64_t n = lower; n <= upper; n += step)
    {
        // --- Generar grafo aleatorio G(n, p) con pesos en [PESO_MIN, PESO_MAX] ---
        std::bernoulli_distribution arista_dist(DENSIDAD);
        Grafo g;
        g.n = static_cast<int>(n);
        for (int u = 0; u < g.n; ++u)
            for (int v = 0; v < g.n; ++v)
                if (u != v && arista_dist(rng))
                    g.aristas.push_back({u, v, peso_dist(rng)});

        // --- Verificacion cruzada: ambos algoritmos deben coincidir ---
        {
            bool cn_base = false, cn_fw = false;
            Matriz D_base = algoritmo_base(g, &cn_base);
            Matriz D_fw   = floyd_warshall(g, &cn_fw);
            if (cn_base != cn_fw) {
                std::cerr << "\n  [ERROR] n=" << n
                          << ": los algoritmos discrepan en deteccion de ciclo\n";
                std::exit(EXIT_FAILURE);
            }
        }

        // ----- Medir algoritmo_base (n ejecuciones de Bellman-Ford) -----
        mean_time = 0.0;
        time_stdev = 0.0;
        for (std::int64_t i = 0; i < runs; ++i)
        {
            display_progress(++ejecutadas, total_runs * 2);
            auto t0 = Clock::now();
            // volatile evita que el compilador elimine la llamada
            volatile auto D = algoritmo_base(g);
            (void)D;
            auto t1 = Clock::now();
            times[i] = Dur(t1 - t0).count();
            mean_time += times[i];
        }
        mean_time /= runs;
        for (std::int64_t i = 0; i < runs; ++i)
        {
            dev = times[i] - mean_time;
            time_stdev += dev * dev;
        }
        time_stdev = std::sqrt(time_stdev / (runs - 1));
        quartiles(times, q);
        csv_base << n << "," << mean_time << "," << time_stdev << ","
                 << q[0] << "," << q[1] << "," << q[2] << "," << q[3] << ","
                 << q[4] << "\n";

        // ----- Medir floyd_warshall (programacion dinamica, Theta(n^3)) -----
        mean_time = 0.0;
        time_stdev = 0.0;
        for (std::int64_t i = 0; i < runs; ++i)
        {
            display_progress(++ejecutadas, total_runs * 2);
            auto t0 = Clock::now();
            volatile auto D = floyd_warshall(g);
            (void)D;
            auto t1 = Clock::now();
            times[i] = Dur(t1 - t0).count();
            mean_time += times[i];
        }
        mean_time /= runs;
        for (std::int64_t i = 0; i < runs; ++i)
        {
            dev = times[i] - mean_time;
            time_stdev += dev * dev;
        }
        time_stdev = std::sqrt(time_stdev / (runs - 1));
        quartiles(times, q);
        csv_fw << n << "," << mean_time << "," << time_stdev << ","
               << q[0] << "," << q[1] << "," << q[2] << "," << q[3] << ","
               << q[4] << "\n";
    }

    std::cerr << "\n\n\033[1;32mExperimento 3 completado.\033[0m\n";
    csv_base.close();
    csv_fw.close();
    return 0;
}
