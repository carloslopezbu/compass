#ifndef UTILS
#define UTILS

#include <algorithm>
#include <concepts>
#include <limits>
#include <random>
#include <stdexcept>
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

    inline size_t weighted_index(const std::vector<float> &weights) {
        if (weights.empty())
            throw std::length_error("The weights sequence must have atlesast one position");

        const float maximun =  *std::max_element(weights.begin(), weights.end());

        if (maximun < limits::fepsilon)
            throw std::runtime_error("Atleast one weight must be above 0.0");

        const size_t size = weights.size() - 1;

        for ( ; ; ) {
            const size_t index = random::uuniform(0, size);

            if (weights[index] < limits::fepsilon)
                continue;

            if (funiform() < weights[index] / maximun)
                return index;
        }
    }

    inline void seed(size_t seed) {
        engine.seed(seed);
    }
}

#endif
