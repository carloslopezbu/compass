#ifndef TRAYECTORY
#define TRAYECTORY

#include "../utils/utils.hpp"
#include <cmath>
#include <concepts>
#include <expected>
#include <functional>
#include <cassert>
#include <string>


namespace compass::trayectory {

    namespace traits {
        template <class Solution>
        concept has_neighbors = compass::core::is_solution<Solution> &&
        requires(Solution &sol) {
            { sol.neighbor() } -> std::same_as<Solution>;
        };
    }


    namespace sa {

        namespace traits {
            template<class CoolingSchedule>
            concept is_cooler = requires(CoolingSchedule &cooling_schedule, float temperature) {
                { cooling_schedule.cooling(temperature) } -> compass::core::is_evaluation;
            };
        }

        namespace helpers {
            inline std::expected<float, std::string> boltzmann(const float delta, const float T) noexcept {
                if (T < compass::utils::limits::fepsilon)
                    return std::unexpected<std::string>("Temperature must be above 0.0");

                return std::exp(-delta / T);
            }
        }

        template<trayectory::traits::has_neighbors Solution,
                 compass::core::evaluator<Solution, float> Evaluator,
                 sa::traits::is_cooler CoolingSchedule>

        class simulated_annealing {

            public:

            struct record {
                float current_temperature;
                float optima;
                size_t iterations;

                record():
                iterations(0){}
            };

            struct parameters {
                float initial_temperature;
                float final_temperature;
                size_t exploration_attempts;

                parameters():
                exploration_attempts(1){}
            };

            using aceptance_criteria = std::function<std::expected<float, std::string>(const float, const float)>;

            simulated_annealing(Solution &start, const Evaluator &evaluator, const CoolingSchedule &cooling_schelude) noexcept :
                _optimal(start),
                _trayectory(start),
                _evaluator(evaluator),
                _cooling_schedule(cooling_schelude),
                _acptcrit(sa::helpers::boltzmann),
                _configured(false)
                {}

            void config(
                const float initial_temperature,
                const float final_temperature,
                const size_t exploration_attempts = 1
            );

            void run();
            void step();

            inline Solution solution() const noexcept {
                return _optimal;
            }

            inline Evaluator &evaluator() const noexcept {
                return _evaluator;
            }

            inline CoolingSchedule &cooling_schedule() const noexcept {
                return _cooling_schedule;
            }

            inline bool is_configured() const noexcept {
                return _configured;
            }

            inline record &get_record() const noexcept {
                return _record;
            }

            inline void update_temperature() noexcept {
                _record.current_temperature = this->_cooling_schedule.cooling(_record.current_temperature);
            }

            aceptance_criteria update_aceptance_criteria(const aceptance_criteria& acptcrit) noexcept {
                 _acptcrit = acptcrit;
            }

            inline bool acept_solution(const float delta, const float T) const noexcept {
                auto prob = this->_acptcrit(delta, T);

                return prob.has_value() && compass::utils::random::funiform() < prob.value();
            }

            private:
            Evaluator _evaluator;
            CoolingSchedule _cooling_schedule;
            aceptance_criteria _acptcrit;
            bool _configured;

            Solution _optimal;
            Solution _trayectory;

            parameters _params;
            record _record;
            float _trayectory_evaluation;
        };
    }

    namespace tabu {
    }
}




template<compass::trayectory::traits::has_neighbors Solution,
         compass::core::evaluator<Solution, float> Evaluator,
         compass::trayectory::sa::traits::is_cooler CoolingSchedule>
    void
    compass::trayectory::sa::simulated_annealing<Solution, Evaluator, CoolingSchedule>::config(
        const float initial_temperature,
        const float final_temperature,
        const size_t exploration_attempts)
    {
        assert(initial_temperature > final_temperature);
        assert(exploration_attempts > 0);

        _params.initial_temperature = initial_temperature;
        _params.final_temperature = final_temperature;
        _params.exploration_attempts = exploration_attempts;


        _record.current_temperature = initial_temperature;
        _record.optima = this->_evaluator.evaluate(_optimal);
        _record.iterations = 0;


        _trayectory_evaluation = this->_evaluator.evaluate(_trayectory);

        _configured = true;
    }


template<compass::trayectory::traits::has_neighbors Solution,
        compass::core::evaluator<Solution, float> Evaluator,
        compass::trayectory::sa::traits::is_cooler CoolingSchedule>
    void
    compass::trayectory::sa::simulated_annealing<Solution, Evaluator, CoolingSchedule>::step()
    {

        assert(_configured);

        if (this->_record.current_temperature < this->_params.final_temperature) {
            // Logg that the algorithm must stop
            return;
        }

        const float T = this->_record.current_temperature;

        for (size_t attempts = 0; attempts < this->_params.exploration_attempts; ++attempts) {
            const Solution candidate = this->_trayectory.neighbor();
            const float candidate_evaluation = this->_evaluator.evaluate(candidate);

            const float delta = candidate_evaluation - this->_trayectory_evaluation;
            const bool upgrades = this->_evaluator.compare(candidate_evaluation, _trayectory_evaluation);


            if (upgrades) {
                this->_trayectory = candidate;
                this->_trayectory_evaluation = candidate_evaluation;


                if (this->_evaluator.compare(candidate_evaluation, this->_record.optima)) {
                    this->_optimal = candidate;
                    this->_record.optima = candidate_evaluation;
                }

                continue;
            }

            if (this->acept_solution(delta, T)) {
                this->_trayectory = candidate;
                this->_trayectory_evaluation = candidate_evaluation;
            }
        }

        this->update_temperature();
        this->_record.iterations++;
    }

    template<compass::trayectory::traits::has_neighbors Solution,
            compass::core::evaluator<Solution, float> Evaluator,
            compass::trayectory::sa::traits::is_cooler CoolingSchedule>
        void
        compass::trayectory::sa::simulated_annealing<Solution, Evaluator, CoolingSchedule>::run()
    {

        for (; _record.current_temperature >= _params.final_temperature  ;) {
            this->step();
        }
    }

#endif
