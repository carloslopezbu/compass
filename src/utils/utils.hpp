#ifndef UTILS
#define UTILS

#include <concepts>
#include <limits>
#include <random>
#include <vector>

namespace compass::core {
    template <class Solution>
    concept is_solution = requires(Solution &sol) {
        true;
    };

    template <class Solution>
    concept is_alterable = requires(Solution &sol) {
        { sol.alterate() } -> std::same_as<Solution>;
    };

    template <typename T>
    concept evaluation = requires {
        requires std::convertible_to<T, float> ||
                  std::same_as<T, std::vector<float>>;
    };

    template <class Evaluator, class Solution>
    concept evaluator =
        evaluation<decltype(std::declval<Evaluator>().evaluate(std::declval<Solution>()))> &&
        requires(Evaluator ev, Solution sol,
                 decltype(ev.evaluate(sol)) e1,
                 decltype(ev.evaluate(sol)) e2)
        {
            { ev.evaluate(sol) } -> evaluation;
            { ev.compare(e1, e2) } -> std::same_as<bool>;
        };
}

namespace compass::utils::limits {
    const float fepsilon = std::numeric_limits<float>::epsilon();
}

namespace compass::utils::random {

    using rng = std::mt19937;

    inline rng engine{std::random_device{}()};

    template<typename RNG>
    inline void update_engine(RNG&& new_engine) {
        engine = std::forward<RNG>(new_engine);
    }

    inline float funiform(float lower = 0.0, float upper = 1.0) {
        std::uniform_real_distribution<float> dist(lower, upper);
        return dist(engine);
    }

    inline double duniform(double lower = 0.0, double upper = 1.0) {
        std::uniform_real_distribution<double> dist(lower, upper);
        return dist(engine);
    }

    inline int iuniform(int lower = 0, int upper = 1) {
        std::uniform_int_distribution<int> dist(lower, upper);
        return dist(engine);
    }

    inline size_t uuniform(size_t lower = 0, size_t upper = 1) {
        std::uniform_int_distribution<size_t> dist(lower, upper);
        return dist(engine);
    }

    inline void seed(size_t seed) {
        engine.seed(seed);
    }
}

#endif
