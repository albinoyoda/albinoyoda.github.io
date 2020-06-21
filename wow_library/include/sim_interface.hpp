#ifndef MODEL_INTERFACE_HPP
#define MODEL_INTERFACE_HPP

#include "sim_input.hpp"
#include "sim_output.hpp"
#include "sim_output_mult.hpp"

class Sim_interface {
public:
    static Sim_output simulate(const Sim_input& input);

    static Sim_output_mult simulate_mult(const Sim_input& input);
};

#endif // INTERFACE_HPP
