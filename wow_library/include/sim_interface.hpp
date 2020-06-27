#ifndef MODEL_INTERFACE_HPP
#define MODEL_INTERFACE_HPP

#include "sim_input.hpp"
#include "sim_output.hpp"
#include "sim_output_mult.hpp"
#include "sim_input_mult.hpp"

class Sim_interface
{
public:
    Sim_output simulate(const Sim_input &input);

    Sim_output_mult simulate_mult(const Sim_input_mult &input);
};

#endif // INTERFACE_HPP
