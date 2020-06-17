#ifndef COVID_OUTPUT_MULT_HPP
#define COVID_OUTPUT_MULT_HPP

#include <utility>

struct Sim_output_mult
{
    Sim_output_mult() = default;

    Sim_output_mult(std::vector<std::string> messages) : messages(std::move(messages)) {}

    std::vector<std::string> messages;
};

#endif // COVID_OUTPUT_MULT_HPP
