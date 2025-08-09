#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "utils/utils.hpp"
#include "trayectory/trayectory.hpp"

// --- Evaluador para TSP ---
struct TspEvaluator {
    float evaluate(const std::vector<int>& tour, const std::vector<std::vector<float>>& dist_matrix) const {
        float total = 0.0f;
        for (size_t i = 0; i < tour.size(); ++i) {
            int a = tour[i];
            int b = tour[(i + 1) % tour.size()];
            total += dist_matrix[a][b];
        }
        return total;
    }

    float evaluate(const struct TspSolution& sol) const;

    bool compare(float n1, float n2) {
        return n1 < n2;
    }
};

// --- Solución para TSP ---
struct TspSolution {
    std::vector<int> tour;
    const std::vector<std::vector<float>>* dist_matrix; // puntero a la matriz de distancias

    TspSolution(const std::vector<std::vector<float>>& matrix)
        : dist_matrix(&matrix)
    {
        tour.resize(matrix.size());
        std::iota(tour.begin(), tour.end(), 0);
        std::shuffle(tour.begin(), tour.end(), compass::utils::random::engine);
    }

    // Genera vecino con un swap simple de dos ciudades
    TspSolution alterate() const {
        TspSolution neigh = *this;
        int i = compass::utils::random::iuniform(0, (int)tour.size() - 1);
        int j = compass::utils::random::iuniform(0, (int)tour.size() - 1);
        std::swap(neigh.tour[i], neigh.tour[j]);
        return neigh;
    }
};

// Implementación de TspEvaluator::evaluate para TspSolution
float TspEvaluator::evaluate(const TspSolution& sol) const {
    return evaluate(sol.tour, *sol.dist_matrix);
}

// --- Cooling schedule ---
struct LinearCooling {
    float cooling(float T) {
        return T * 0.95f;
    }
};

int main() {
    constexpr int NUM_CITIES = 50;
    std::vector<std::vector<float>> dist_matrix(NUM_CITIES, std::vector<float>(NUM_CITIES));

    // Generar distancias aleatorias simétricas
    for (int i = 0; i < NUM_CITIES; ++i) {
        for (int j = i; j < NUM_CITIES; ++j) {
            if (i == j) {
                dist_matrix[i][j] = 0.0f;
            } else {
                float dist = compass::utils::random::funiform(1.0f, 100.0f);
                dist_matrix[i][j] = dist_matrix[j][i] = dist;
            }
        }
    }

    TspSolution init(dist_matrix);
    TspEvaluator evaluator;
    LinearCooling cooling;


    const float T0 = 1000.0f;
    const float Tf = 0.1f;
    const size_t attempts_per_temp = 100;

    compass::trayectory::sa::simulated_annealing sa(T0, Tf, attempts_per_temp);


    auto best = sa.run(cooling, evaluator, init);

    std::cout << "Mejor distancia: " << evaluator.evaluate(best) << "\n";
    std::cout << "Tour: ";
    for (auto city : best.tour) std::cout << city << " ";
    std::cout << "\n";

    return 0;
}
