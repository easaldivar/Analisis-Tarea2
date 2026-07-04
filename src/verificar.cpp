// verificar.cpp -- Verificacion de correctitud de las implementaciones
// (pregunta 2.1). Ejecuta dos baterias de pruebas:
//
//   Bateria 1: instancias con solucion conocida. Cada archivo
//     tests/casos/X.grafo tiene un tests/casos/X.esperado con la matriz de
//     distancias correcta (calculada a mano o tomada de la literatura; ver
//     informe, seccion 2.3) o la palabra CICLO_NEGATIVO si la instancia
//     contiene un ciclo de peso negativo. Ambos algoritmos deben reproducir
//     exactamente lo esperado.
//
//   Bateria 2: verificacion cruzada sobre grafos aleatorios reproducibles
//     (semilla fija). El Algoritmo Base y Floyd-Warshall usan tecnicas
//     distintas; que coincidan entrada a entrada en cientos de instancias
//     variadas es evidencia fuerte de correctitud de ambos.
//
// Uso (desde la raiz del repositorio):
//   ./verificar [directorio_de_casos]      (por defecto "tests/casos")
//
// Codigo de salida: 0 si todas las pruebas pasan, 1 en caso contrario.

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "apsp.hpp"

namespace fs = std::filesystem;

// Tolerancia al comparar distancias. Los pesos de todas las pruebas son
// multiplos de 0.25, exactamente representables en double, de modo que las
// sumas son exactas y en la practica la diferencia debe ser exactamente 0;
// la tolerancia queda solo como resguardo.
const double TOL = 1e-9;

bool iguales(double a, double b) {
    if (std::isinf(a) || std::isinf(b)) return std::isinf(a) && std::isinf(b);
    return std::fabs(a - b) <= TOL;
}

bool matrices_iguales(const Matriz& A, const Matriz& B) {
    if (A.size() != B.size()) return false;
    for (std::size_t i = 0; i < A.size(); ++i)
        for (std::size_t j = 0; j < A.size(); ++j)
            if (!iguales(A[i][j], B[i][j])) return false;
    return true;
}

// Resultado esperado de una instancia: o bien "hay ciclo negativo", o bien
// una matriz de n x n donde el token INF denota "no existe camino".
struct Esperado {
    bool ciclo_negativo = false;
    Matriz D;
};

Esperado leer_esperado(const std::string& ruta, int n) {
    std::ifstream entrada(ruta);
    if (!entrada) throw std::runtime_error("No se pudo abrir el archivo: " + ruta);
    Esperado e;
    std::string tok;
    entrada >> tok;
    if (tok == "CICLO_NEGATIVO") {
        e.ciclo_negativo = true;
        return e;
    }
    e.D.assign(n, std::vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != 0 || j != 0) entrada >> tok;  // el primer token ya fue leido
            e.D[i][j] = (tok == "INF") ? INF : std::stod(tok);
        }
    }
    if (!entrada) throw std::runtime_error("Formato invalido en: " + ruta);
    return e;
}

int main(int argc, char** argv) {
    const std::string dir = (argc > 1) ? argv[1] : "tests/casos";
    int fallas = 0;

    // ---------- Bateria 1: instancias con solucion conocida ----------
    std::printf("== Bateria 1: instancias con solucion conocida (%s) ==\n", dir.c_str());
    std::vector<fs::path> casos;
    for (const auto& entrada : fs::directory_iterator(dir))
        if (entrada.path().extension() == ".grafo") casos.push_back(entrada.path());
    std::sort(casos.begin(), casos.end());

    for (const fs::path& ruta : casos) {
        Grafo g = leer_grafo(ruta.string());
        fs::path ruta_esperado = ruta;
        ruta_esperado.replace_extension(".esperado");
        Esperado esperado = leer_esperado(ruta_esperado.string(), g.n);

        bool cn_base = false, cn_fw = false;
        Matriz D_base = algoritmo_base(g, &cn_base);
        Matriz D_fw = floyd_warshall(g, &cn_fw);

        bool ok;
        if (esperado.ciclo_negativo) {
            // Ambos algoritmos deben detectar el ciclo negativo.
            ok = cn_base && cn_fw;
        } else {
            // Ninguno debe reportar ciclo negativo y ambas matrices deben
            // coincidir con la esperada (y, en consecuencia, entre si).
            ok = !cn_base && !cn_fw && matrices_iguales(D_base, esperado.D) &&
                 matrices_iguales(D_fw, esperado.D);
        }
        std::printf("  [%s] %s (n=%d, m=%d)\n", ok ? "OK" : "FALLA",
                    ruta.filename().string().c_str(), g.n,
                    static_cast<int>(g.aristas.size()));
        if (!ok) ++fallas;
    }

    // ---------- Bateria 2: verificacion cruzada aleatoria ----------
    const int PRUEBAS = 200;
    std::printf("== Bateria 2: verificacion cruzada sobre %d grafos aleatorios ==\n",
                PRUEBAS);
    std::mt19937 rng(20260706u);  // semilla fija: pruebas reproducibles
    const double DENSIDADES[] = {0.1, 0.3, 0.6, 0.9};
    int con_ciclo = 0, fallas_aleatorias = 0;

    for (int t = 0; t < PRUEBAS; ++t) {
        // Grafo aleatorio G(n, p): cada arista (u,v), u != v, existe con
        // probabilidad p. Pruebas pares: pesos en [0, 10]; pruebas impares:
        // pesos en [-3, 10] (pueden aparecer ciclos negativos, que ambos
        // algoritmos deben senalar). Los pesos son multiplos de 0.25.
        int n = std::uniform_int_distribution<int>(1, 40)(rng);
        double p = DENSIDADES[t % 4];
        int peso_minimo = (t % 2 == 0) ? 0 : -12;  // en cuartos
        std::bernoulli_distribution existe_arista(p);
        std::uniform_int_distribution<int> cuartos(peso_minimo, 40);

        Grafo g;
        g.n = n;
        for (int u = 0; u < n; ++u)
            for (int v = 0; v < n; ++v)
                if (u != v && existe_arista(rng))
                    g.aristas.push_back({u, v, cuartos(rng) / 4.0});

        bool cn_base = false, cn_fw = false;
        Matriz D_base = algoritmo_base(g, &cn_base);
        Matriz D_fw = floyd_warshall(g, &cn_fw);

        // Ambos deben coincidir en la deteccion de ciclos negativos; si no
        // hay ciclo negativo, las matrices deben ser identicas (con ciclo
        // negativo las distancias no estan bien definidas y no se comparan).
        bool ok = (cn_base == cn_fw) && (cn_base || matrices_iguales(D_base, D_fw));
        if (cn_base && cn_fw) ++con_ciclo;
        if (!ok) {
            std::printf("  [FALLA] prueba aleatoria %d: n=%d, p=%.1f, m=%d\n", t, n,
                        p, static_cast<int>(g.aristas.size()));
            ++fallas_aleatorias;
        }
    }
    std::printf("  %d/%d pruebas correctas (%d instancias con ciclo negativo, "
                "detectado por ambos algoritmos)\n",
                PRUEBAS - fallas_aleatorias, PRUEBAS, con_ciclo);
    fallas += fallas_aleatorias;

    if (fallas == 0)
        std::printf("== RESULTADO: todas las pruebas pasaron ==\n");
    else
        std::printf("== RESULTADO: %d prueba(s) FALLARON ==\n", fallas);
    return (fallas == 0) ? 0 : 1;
}
