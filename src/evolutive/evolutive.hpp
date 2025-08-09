#ifndef EVOLUTIVE
#define EVOLUTIVE

#include "../utils/utils.hpp"
#include <concepts>
#include <cstddef>

namespace compass::evolutive::traits {

    template<class Selector, class Population>
    concept selector = requires(Selector &sel, const Population &pop) {
        { sel.select(pop) } -> std::same_as<size_t>;

        requires compass::core::is_solution<typename Population::value_type>;
    };

    template<class Crossover, class Solution>
    concept crossover = requires(Crossover &cx, const Solution &p1, const Solution &p2) {
        { cx.crossover(p1, p2) } -> std::same_as<Solution>;
    };

    template<class Mutator, class Solution>
    concept mutator = requires(Mutator &mut, Solution &sol) {
        { mut.mutate(sol) } -> std::same_as<void>;
    };

    template<class Replacer, class Population>
    concept replacer = requires(Replacer &rep, Population &pop1, const Population &pop2) {
        { rep.select(pop1, pop2) } -> std::same_as<Population>;
    };

}

namespace compass::evolutive::ga {

}

namespace compass::evolutive::gp {

}

namespace compass::evolutive::de {

}

namespace compass::evolutive::swarm {

    namespace aco {

    }

    namespace pso {

    }

    namespace gsa {

    }
}

#endif
