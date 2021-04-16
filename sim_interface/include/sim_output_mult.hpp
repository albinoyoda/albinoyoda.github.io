#ifndef COVID_OUTPUT_MULT_HPP
#define COVID_OUTPUT_MULT_HPP

#include <string>
#include <vector>

struct Sim_output_mult
{
    Sim_output_mult() = default;

    Sim_output_mult(std::vector<std::string> messages) : messages(std::move(messages)) {}

    std::vector<std::string> messages;
};

#endif // COVID_OUTPUT_MULT_HPP
