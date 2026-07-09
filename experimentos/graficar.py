# graficar.py - Graficos comparativos para Pregunta 2.2
#
# Genera 4 gráficos en escala lineal, uno por experimento.
# Cada gráfico compara algoritmo_base y floyd_warshall con:
#   - Puntos medidos + barras de error (±1 stdev)
#   - Curvas de ajuste teórico (mínimos cuadrados)
#
# Requiere: pip install pandas numpy matplotlib

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

RESULTADOS = Path("resultados")
SALIDA     = Path("resultados/graficos")
SALIDA.mkdir(parents=True, exist_ok=True)

# ===== Funciones de ajuste =====

def fit_through_origin(x, y):
    """Ajusta y = a*x por minimos cuadrados (intercepto 0)."""
    a = np.sum(x * y) / np.sum(x * x)
    return a

def fit_constant(y):
    """Ajusta y = a (constante)."""
    return np.mean(y)

def r2(y_real, y_pred):
    """Coeficiente de determinacion R²."""
    ss_res = np.sum((y_real - y_pred) ** 2)
    ss_tot = np.sum((y_real - np.mean(y_real)) ** 2)
    return 1 - ss_res / ss_tot

# ===== Configuración global =====

plt.rcParams.update({
    "figure.figsize": (10, 6),
    "figure.dpi": 120,
    "font.size": 12,
    "axes.titlesize": 14,
    "axes.labelsize": 13,
    "legend.fontsize": 11,
})

COLOR_BASE = "#2196F3"   # azul
COLOR_FW   = "#FF5722"   # naranja
COLOR_BASE_FIT = "#0D47A1"  # azul oscuro (curva ajustada)
COLOR_FW_FIT   = "#BF360C"  # naranja oscuro

# ============================================================
# Experimento 1: n creciente, densidad fija (p = 0.3)
#   FW:     t = a * n^3
#   Base:   t = b * n^2 * m   (con m medido ≈ 0.3*n*(n-1))
# ============================================================
def graficar_e1():
    base = pd.read_csv(RESULTADOS / "base_e1_us.csv")
    fw   = pd.read_csv(RESULTADOS / "fw_e1_us.csv")

    n = base["n"].values
    # m no esta en el CSV de E1; lo estimamos: m ≈ 0.3 * n * (n-1)
    m_est = 0.3 * n * (n - 1)

    fig, ax = plt.subplots()

    # --- Puntos medidos ---
    ax.errorbar(n, base["t_mean"], yerr=base["t_stdev"],
                fmt="o", capsize=3, markersize=5,
                color=COLOR_BASE, label="Algoritmo Base (medido)")
    ax.errorbar(n, fw["t_mean"], yerr=fw["t_stdev"],
                fmt="s", capsize=3, markersize=5,
                color=COLOR_FW, label="Floyd-Warshall (medido)")

    # --- Ajustes teóricos ---
    # FW: t = a * n^3
    x_fw = n ** 3
    a_fw = fit_through_origin(x_fw, fw["t_mean"].values)
    n_smooth = np.linspace(n[0], n[-1], 200)
    ax.plot(n_smooth, a_fw * n_smooth**3, "--", color=COLOR_FW_FIT, linewidth=2,
            label=f"FW ajustado: $t = {a_fw:.2e} \\cdot n^3$\n$R^2 = {r2(fw['t_mean'], a_fw * x_fw):.4f}$")

    # Base: t = b * n^2 * m
    x_base = n**2 * m_est
    b_base = fit_through_origin(x_base, base["t_mean"].values)
    y_fit_base = b_base * n_smooth**2 * (0.3 * n_smooth * (n_smooth - 1))
    ax.plot(n_smooth, y_fit_base, "--", color=COLOR_BASE_FIT, linewidth=2,
            label=f"Base ajustado: $t = {b_base:.2e} \\cdot n^2 m$\n$R^2 = {r2(base['t_mean'], b_base * x_base):.4f}$")

    ax.set_xlabel("$n$ (número de nodos)")
    ax.set_ylabel("Tiempo ($\\mu$s)")
    ax.set_title("Experimento 1: $n$ creciente, $p = 0.3$")
    ax.legend(loc="upper left")
    ax.grid(True, alpha=0.3)
    fig.tight_layout()
    fig.savefig(SALIDA / "e1_n_creciente.pdf")
    fig.savefig(SALIDA / "e1_n_creciente.png")
    plt.close(fig)
    print(f"E1: a_fw={a_fw:.4e}, b_base={b_base:.4e}")


# ============================================================
# Experimento 2: densidad creciente, n fijo (n = 100)
#   FW:     t ≈ constante (no depende de m)
#   Base:   t = b * m  (con n^2 constante = 10000)
#   CSV tiene columnas: densidad,m,t_mean,...
# ============================================================
def graficar_e2():
    base = pd.read_csv(RESULTADOS / "base_e2_us.csv")
    fw   = pd.read_csv(RESULTADOS / "fw_e2_us.csv")

    m = base["m"].values
    densidad = base["densidad"].values

    fig, ax = plt.subplots()

    # --- Puntos medidos ---
    ax.errorbar(m, base["t_mean"], yerr=base["t_stdev"],
                fmt="o", capsize=3, markersize=5,
                color=COLOR_BASE, label="Algoritmo Base (medido)")
    ax.errorbar(m, fw["t_mean"], yerr=fw["t_stdev"],
                fmt="s", capsize=3, markersize=5,
                color=COLOR_FW, label="Floyd-Warshall (medido)")

    # --- Ajustes teóricos ---
    # FW: constante
    c_fw = fit_constant(fw["t_mean"].values)
    m_smooth = np.linspace(m[0], m[-1], 200)
    ax.axhline(y=c_fw, color=COLOR_FW_FIT, linestyle="--", linewidth=2,
               label=f"FW ajustado: $t = {c_fw:.2f}$ (constante)")

    # Base: t = b * m
    b_base = fit_through_origin(m, base["t_mean"].values)
    ax.plot(m_smooth, b_base * m_smooth, "--", color=COLOR_BASE_FIT, linewidth=2,
            label=f"Base ajustado: $t = {b_base:.2e} \\cdot m$\n$R^2 = {r2(base['t_mean'], b_base * m):.4f}$")

    ax.set_xlabel("$m$ (número de aristas)")
    ax.set_ylabel("Tiempo ($\\mu$s)")
    ax.set_title("Experimento 2: densidad creciente, $n = 100$")
    ax.legend(loc="upper left")
    ax.grid(True, alpha=0.3)
    fig.tight_layout()
    fig.savefig(SALIDA / "e2_densidad.pdf")
    fig.savefig(SALIDA / "e2_densidad.png")
    plt.close(fig)
    print(f"E2: c_fw={c_fw:.4e}, b_base={b_base:.4e}")


# ============================================================
# Experimento 3: n creciente, pesos negativos (p = 0.15)
#   Misma estructura que E1.
#   FW:     t = a * n^3
#   Base:   t = b * n^2 * m
# ============================================================
def graficar_e3():
    base = pd.read_csv(RESULTADOS / "base_e3_us.csv")
    fw   = pd.read_csv(RESULTADOS / "fw_e3_us.csv")

    n = base["n"].values
    m_est = 0.15 * n * (n - 1)

    fig, ax = plt.subplots()

    ax.errorbar(n, base["t_mean"], yerr=base["t_stdev"],
                fmt="o", capsize=3, markersize=5,
                color=COLOR_BASE, label="Algoritmo Base (medido)")
    ax.errorbar(n, fw["t_mean"], yerr=fw["t_stdev"],
                fmt="s", capsize=3, markersize=5,
                color=COLOR_FW, label="Floyd-Warshall (medido)")

    # FW: t = a * n^3
    x_fw = n ** 3
    a_fw = fit_through_origin(x_fw, fw["t_mean"].values)
    n_smooth = np.linspace(n[0], n[-1], 200)
    ax.plot(n_smooth, a_fw * n_smooth**3, "--", color=COLOR_FW_FIT, linewidth=2,
            label=f"FW ajustado: $t = {a_fw:.2e} \\cdot n^3$\n$R^2 = {r2(fw['t_mean'], a_fw * x_fw):.4f}$")

    # Base: t = b * n^2 * m
    x_base = n**2 * m_est
    b_base = fit_through_origin(x_base, base["t_mean"].values)
    y_fit_base = b_base * n_smooth**2 * (0.15 * n_smooth * (n_smooth - 1))
    ax.plot(n_smooth, y_fit_base, "--", color=COLOR_BASE_FIT, linewidth=2,
            label=f"Base ajustado: $t = {b_base:.2e} \\cdot n^2 m$\n$R^2 = {r2(base['t_mean'], b_base * x_base):.4f}$")

    ax.set_xlabel("$n$ (número de nodos)")
    ax.set_ylabel("Tiempo ($\\mu$s)")
    ax.set_title("Experimento 3: pesos en $[-2, 10]$, $p = 0.15$")
    ax.legend(loc="upper left")
    ax.grid(True, alpha=0.3)
    fig.tight_layout()
    fig.savefig(SALIDA / "e3_pesos_negativos.pdf")
    fig.savefig(SALIDA / "e3_pesos_negativos.png")
    plt.close(fig)
    print(f"E3: a_fw={a_fw:.4e}, b_base={b_base:.4e}")


# ============================================================
# Experimento 4: grafos completos (p = 1.0, m = n*(n-1))
#   FW:     t = a * n^3
#   Base:   t = b * n^2 * m = b * n^2 * n*(n-1) ≈ b * n^4
# ============================================================
def graficar_e4():
    base = pd.read_csv(RESULTADOS / "base_e4_us.csv")
    fw   = pd.read_csv(RESULTADOS / "fw_e4_us.csv")

    n = base["n"].values
    m_real = n * (n - 1)  # grafo completo

    fig, ax = plt.subplots()

    ax.errorbar(n, base["t_mean"], yerr=base["t_stdev"],
                fmt="o", capsize=3, markersize=5,
                color=COLOR_BASE, label="Algoritmo Base (medido)")
    ax.errorbar(n, fw["t_mean"], yerr=fw["t_stdev"],
                fmt="s", capsize=3, markersize=5,
                color=COLOR_FW, label="Floyd-Warshall (medido)")

    # FW: t = a * n^3
    x_fw = n ** 3
    a_fw = fit_through_origin(x_fw, fw["t_mean"].values)
    n_smooth = np.linspace(n[0], n[-1], 200)
    ax.plot(n_smooth, a_fw * n_smooth**3, "--", color=COLOR_FW_FIT, linewidth=2,
            label=f"FW ajustado: $t = {a_fw:.2e} \\cdot n^3$\n$R^2 = {r2(fw['t_mean'], a_fw * x_fw):.4f}$")

    # Base: t = b * n^2 * m = b * n^2 * n*(n-1)
    x_base = n**2 * m_real
    b_base = fit_through_origin(x_base, base["t_mean"].values)
    y_fit_base = b_base * n_smooth**2 * (n_smooth * (n_smooth - 1))
    ax.plot(n_smooth, y_fit_base, "--", color=COLOR_BASE_FIT, linewidth=2,
            label=f"Base ajustado: $t = {b_base:.2e} \\cdot n^2 m$\n$R^2 = {r2(base['t_mean'], b_base * x_base):.4f}$")

    ax.set_xlabel("$n$ (número de nodos)")
    ax.set_ylabel("Tiempo ($\\mu$s)")
    ax.set_title("Experimento 4: grafos completos ($p = 1.0$)")
    ax.legend(loc="upper left")
    ax.grid(True, alpha=0.3)
    fig.tight_layout()
    fig.savefig(SALIDA / "e4_grafos_completos.pdf")
    fig.savefig(SALIDA / "e4_grafos_completos.png")
    plt.close(fig)
    print(f"E4: a_fw={a_fw:.4e}, b_base={b_base:.4e}")


# ============================================================
# Main
# ============================================================
if __name__ == "__main__":
    print("Generando gráficos...\n")

    graficar_e1()
    graficar_e2()
    graficar_e3()
    graficar_e4()

    print(f"\nGráficos guardados en {SALIDA.resolve()}/")
