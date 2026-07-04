#pragma once
// grafo.hpp -- Representacion de grafos dirigidos con pesos reales.
// No se emplean librerias de grafos: solo contenedores estandar de C++.

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

// Arista dirigida u -> v con peso w.
struct Arista {
    int u;
    int v;
    double w;
};

// Grafo dirigido representado por su numero de nodos y su lista de aristas.
// Los nodos se identifican con los enteros 0, 1, ..., n-1.
struct Grafo {
    int n = 0;
    std::vector<Arista> aristas;
};

// Lee un grafo desde un archivo de texto con el formato:
//   n m
//   u v w    (m lineas: arista u -> v de peso w; nodos 0-indexados)
inline Grafo leer_grafo(const std::string& ruta) {
    std::ifstream entrada(ruta);
    if (!entrada) throw std::runtime_error("No se pudo abrir el archivo: " + ruta);
    Grafo g;
    int m = 0;
    entrada >> g.n >> m;
    g.aristas.reserve(m);
    for (int i = 0; i < m; ++i) {
        Arista a;
        entrada >> a.u >> a.v >> a.w;
        g.aristas.push_back(a);
    }
    if (!entrada) throw std::runtime_error("Formato invalido en: " + ruta);
    return g;
}
