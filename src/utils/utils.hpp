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

    template <typename T>
    concept is_evaluation = requires {
        requires std::convertible_to<T, float>;
    };

    template <class Evaluator, class Solution, class Evaluation>
    concept evaluator = is_solution<Solution> &&
    requires(Evaluator &evaluator, const Solution &solution) { { evaluator.evaluate(solution) } -> is_evaluation; } &&
    is_evaluation<Evaluation> &&
    requires(Evaluator &evaluator, const Evaluation ev1, const Evaluation ev2){
        { evaluator.compare(ev1, ev2) } -> std::same_as<bool>;
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
