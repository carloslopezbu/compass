#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "utils/utils.hpp"
#include "trayectory/trayectory.hpp"


struct TSP {
    std::vector<int> tour; // orden de ciudades
    const std::vector<std::vector<float>>* dist = nullptr; // puntero a la matriz de distancias

    // constructores
    TSP() = default;
    TSP(const std::vector<int>& t, const std::vector<std::vector<float>>* d) : tour(t), dist(d) {}

    // required by compass::trayectory::traits::has_neighbors
    TSP neighbor() {
        // generamos vecino por swap de dos posiciones aleatorias
        TSP n = *this;
        const size_t ncity = n.tour.size();
        if (ncity < 2) return n;

        size_t i = compass::utils::random::uuniform(0, ncity - 1);
        size_t j = compass::utils::random::uuniform(0, ncity - 1);
        while (j == i) j = compass::utils::random::uuniform(0, ncity - 1);

        std::swap(n.tour[i], n.tour[j]);
        return n;
    }
};

// Evaluator para TSP: minimizamos la longitud total del tour
struct TSPEvaluator {
    const std::vector<std::vector<float>>* dist = nullptr;
    TSPEvaluator() = default;
    TSPEvaluator(const std::vector<std::vector<float>>* d) : dist(d) {}

    // required: evaluate(solution) -> convertible_to<float>
    float evaluate(const TSP &s) {
        if (!dist) return std::numeric_limits<float>::infinity();
        const auto &mat = *dist;
        float sum = 0.0f;
        for (size_t i = 0; i < s.tour.size(); ++i) {
            size_t a = s.tour[i];
            size_t b = s.tour[(i + 1) % s.tour.size()];
            sum += mat[a][b];
        }
        return sum;
    }

    // required: compare(ev1, ev2) -> bool (true si ev1 es "mejor" que ev2)
    // Para minimización: mejor cuando menor
    bool compare(const float ev1, const float ev2) {
        return ev1 < ev2;
    }
};

// Cooling schedule: exponencial T <- alpha * T
struct ExponentialCooling {
    float alpha;
    ExponentialCooling(float a = 0.95f) : alpha(a) {}

    // nota: concept espera floating return y no-const member function
    float cooling(float T) {
        return T * alpha;
    }
};

std::vector<std::vector<float>> generar_grafo(size_t n, float min_dist = 10.0f, float max_dist = 50.0f) {
    std::vector<std::vector<float>> dist(n, std::vector<float>(n, 0.0f));



    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            float d = compass::utils::random::funiform(10, 20000);
            dist[i][j] = d;
            dist[j][i] = d; // simétrico
        }
    }

    return dist;
}


int main()
{

    std::vector<std::vector<float>> dist = generar_grafo(5000);

        const size_t ncity = dist.size();
        std::vector<int> initial_tour(ncity);
        for (size_t i=0;i<ncity;++i) initial_tour[i] = (int)i;

        // permutation inicial aleatoria
        std::shuffle(initial_tour.begin(), initial_tour.end(), compass::utils::random::engine);

        TSP start(initial_tour, &dist);
        TSPEvaluator evaluator(&dist);
        ExponentialCooling cooling(0.90f);

        const float eval2 = evaluator.evaluate(start);


        float T0 = 100.0f;
        float Tf = 1e-3f;
    auto sa = compass::trayectory::sa::simulated_annealing<TSP, TSPEvaluator, ExponentialCooling>(start, evaluator, cooling);

    sa.config(1000.0f, 10.0f, 100);

    sa.run();

    const auto sol = sa.solution();
    const float eval = evaluator.evaluate(sol);

    std::cout << eval << " " << eval2;



}
