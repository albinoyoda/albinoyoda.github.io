#ifndef COVID_OUTPUT_MULT_HPP
#define COVID_OUTPUT_MULT_HPP

#include <string>
#include <vector>

struct Sim_output_mult
{
    Sim_output_mult() = default;

    Sim_output_mult(std::vector<std::string> messages, std::vector<std::vector<std::string>> keepers)
        : messages(std::move(messages)), keepers(std::move(keepers))
    {
    }

    std::vector<std::string> messages;
    std::vector<std::vector<std::string>> keepers;
};

#endif // COVID_OUTPUT_MULT_HPP
