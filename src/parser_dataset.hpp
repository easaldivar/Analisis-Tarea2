#pragma once
// parser_dataset.hpp
// Lectura de datasets en formatos MatrixMarket (.mtx)
// y edge-list (.edges), usados en la pregunta 2.3.

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "grafo.hpp"

// Lee un grafo desde un archivo MatrixMarket (.mtx), formato coordinate.
// Soporta los tipos "general" y "symmetric":
//   - general:     cada linea u v w define una arista dirigida u -> v.
//   - symmetric:   cada linea u v w define dos aristas: u -> v y v -> u
//                   (si u != v). Si u == v, solo se agrega una.
// Los nodos vienen 1-indexados y se convierten a 0-indexados.
// Lineas que empiezan con '%' son comentarios y se ignoran.
inline Grafo leer_mtx(const std::string& ruta) {
    std::ifstream entrada(ruta);
    if (!entrada)
        throw std::runtime_error("No se pudo abrir el archivo: " + ruta);

    std::string linea;
    bool simetrico = false;

    // Leer header: %%MatrixMarket ...
    while (std::getline(entrada, linea)) {
        // Ignorar comentarios
        if (linea.empty()) continue;
        if (linea[0] == '%') {
            // Detectar simetria en la primera linea de comentario del header
            if (linea.find("symmetric") != std::string::npos)
                simetrico = true;
            continue;
        }
        // Primera linea no-comentario = dimensiones
        break;
    }

    // Leer dimensiones: nrows ncols nentries [optional fields]
    std::istringstream dims(linea);
    int n = 0, filas = 0, entradas = 0;
    dims >> filas >> n >> entradas;
    if (!dims)
        throw std::runtime_error("Header invalido en MTX: " + ruta);
    if (filas != n)
        throw std::runtime_error("MTX debe ser cuadrado (nrows == ncols): " + ruta);

    Grafo g;
    g.n = n;
    g.aristas.reserve(simetrico ? entradas * 2 : entradas);

    int u, v;
    double w;
    while (entrada >> u >> v >> w) {
        // MTX es 1-indexado
        --u;
        --v;
        g.aristas.push_back({u, v, w});
        if (simetrico && u != v)
            g.aristas.push_back({v, u, w});
    }

    if (entrada.bad())
        throw std::runtime_error("Error leyendo entradas de MTX: " + ruta);

    return g;
}

// Lee un grafo desde un archivo .edges, con formato:
//   u v w
// (una arista dirigida por linea, nodos 0-indexados).
// Determina n como max(u, v) + 1 entre todas las aristas leidas.
// Si los nodos no empiezan en 0, se reindexan compactamente a [0, k-1]
// donde k es la cantidad de nodos distintos.
inline Grafo leer_edges(const std::string& ruta) {
    std::ifstream entrada(ruta);
    if (!entrada)
        throw std::runtime_error("No se pudo abrir el archivo: " + ruta);

    // Primera pasada: recolectar nodos unicos
    std::unordered_map<int, int> reindex;
    std::vector<std::tuple<int, int, double>> aristas_raw;
    int u, v;
    double w;

    while (entrada >> u >> v >> w) {
        aristas_raw.emplace_back(u, v, w);
        if (reindex.find(u) == reindex.end())
            reindex[u] = static_cast<int>(reindex.size());
        if (reindex.find(v) == reindex.end())
            reindex[v] = static_cast<int>(reindex.size());
    }

    if (entrada.bad())
        throw std::runtime_error("Error leyendo aristas de: " + ruta);

    // Segunda pasada: construir grafo con nodos reindexados
    Grafo g;
    g.n = static_cast<int>(reindex.size());
    g.aristas.reserve(aristas_raw.size());
    for (const auto& [orig_u, orig_v, peso] : aristas_raw)
        g.aristas.push_back({reindex[orig_u], reindex[orig_v], peso});

    return g;
}
