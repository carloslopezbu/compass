#ifndef TRAYECTORY
#define TRAYECTORY

#include "../utils/utils.hpp"
#include <cmath>
#include <expected>
#include <stdexcept>
#include <string>


namespace compass::trayectory {

    namespace sa {

        namespace traits {
            template<class Cooler>
            concept is_cooler = requires(Cooler &cooling_schedule, float temperature) {
                { cooling_schedule.cooling(temperature) } -> compass::core::evaluation;
            };

            template <class Solution>
            concept impl = compass::core::is_solution<Solution> && compass::core::is_alterable<Solution>;

        }

        namespace helpers {
            inline std::expected<float, std::string> boltzmann(const float delta, const float T) {
                if (T < compass::utils::limits::fepsilon)
                    return std::unexpected<std::string>("Temperature must be above 0.0");

                return std::exp(-delta / T);
            }
        }

        /**
         * @class Simulated Annealing
         * @brief Aplies SA
         *
         * @param
         */
        class simulated_annealing {

            public:

            explicit simulated_annealing(const float T0,
                                const float Tf,
                                const size_t exploration_attempts = 1)
            {

                if (T0 < Tf)
                    throw std::range_error("T0 must be lower than Tf");

                _T0 = T0;
                _T = T0;
                _Tf = Tf;
                _exploration_attempts = exploration_attempts;
                _iterations = 0;
            }

            inline float T0() const {
                return _T0;
            }

            inline float Tf() const {
                return _Tf;
            }

            inline size_t exploration_attempts() const {
                return _exploration_attempts;
            }

            inline compass::core::is_solution auto run(
                sa::traits::is_cooler auto &cooling_schedule,
                auto &evaluator,
                sa::traits::impl auto start
            )
            requires sa::traits::is_cooler<decltype(cooling_schedule)> &&
                     compass::core::evaluator<decltype(evaluator), decltype(start)> &&
                     sa::traits::impl<decltype(start)>
            {
                auto current_solution = start;
                auto best_solution = start;

                float current_eval = evaluator.evaluate(current_solution);
                float best_eval = current_eval;

                for ( ; _T >= _Tf; ++_iterations) {
                    for (size_t attempts = 0; attempts < _exploration_attempts; ++attempts) {
                        const auto candidate = current_solution.alterate();
                        const float candidate_eval = evaluator.evaluate(candidate);

                        const float delta = candidate_eval - best_eval;
                        const bool upgrades = evaluator.compare(delta, compass::utils::limits::fepsilon);

                        if (upgrades) {
                            best_solution = candidate;
                            best_eval = candidate_eval;
                            current_solution = candidate;
                            current_eval = candidate_eval;
                            continue;
                        }

                        const auto boltz = helpers::boltzmann(delta, _T);
                        if (boltz.has_value() && evaluator.compare(boltz.value(), delta)) {
                            current_solution = candidate;
                            current_eval = candidate_eval;
                        } else if (!boltz.has_value()) {
                            return best_solution;
                        }
                    }
                    _T = cooling_schedule.cooling(_T);
                }
                return best_solution;
            }

            private:
            float _T0;
            float _Tf;
            float _T;
            size_t _exploration_attempts;
            size_t _iterations;
        };
    }

    namespace tabu {
    }
}

#endif
