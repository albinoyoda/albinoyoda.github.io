#ifndef COVID_OUTPUT_HPP
#define COVID_OUTPUT_HPP

#include <utility>

struct Sim_output
{
    Sim_output() = default;

    Sim_output(std::vector<double> dmg_batches, std::vector<double> dmg_sources, std::vector<std::string> aura_uptimes,
               double mean_dps, double std_dps, std::vector<std::string> messages)
            : dmg_batches(std::move(dmg_batches)), dmg_sources(std::move(dmg_sources)),
            aura_uptimes(std::move(aura_uptimes)), mean_dps(mean_dps), std_dps(std_dps),
            messages(std::move(messages)) {}

    std::vector<double> dmg_batches;
    std::vector<double> dmg_sources;
    std::vector<std::string> aura_uptimes;
    double mean_dps{};
    double std_dps{};
    std::vector<std::string> messages;
};

#endif // COVID_OUTPUT_HPP
