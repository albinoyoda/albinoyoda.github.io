#ifndef MODEL_INTERFACE_HPP
#define MODEL_INTERFACE_HPP

#include "sim_input.hpp"
#include "sim_output.hpp"

class Sim_interface
{
public:
    Sim_output simulate(const Sim_input& input);
};

#endif // INTERFACE_HPP
