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
        return T * 0.99f;
    }
};

int main() {

    std::cout << compass::utils::random::weighted_index({50.0, 70.0, 20.0, 40.0, 60.0}) << std::endl;

    return 0;
}
