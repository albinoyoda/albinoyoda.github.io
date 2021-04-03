#ifndef COVID_OUTPUT_HPP
#define COVID_OUTPUT_HPP

#include <string>
#include <vector>

struct Sim_output
{
    Sim_output() = default;

    Sim_output(
            std::vector<int> hist_x,
            std::vector<int> hist_y,
            std::vector<double> dmg_sources,
            std::vector<std::string> time_lapse_names,
            std::vector<std::vector<double>> damage_time_lapse,
            std::vector<std::string> aura_uptimes,
            std::vector<std::string> use_effect_order_string,
            std::vector<std::string> proc_counter,
            std::vector<std::string> stat_weights,
            std::vector<std::string> extra_stats,
            std::vector<double> mean_dps,
            std::vector<double> std_dps,
            std::vector<std::string> messages)
            :
            hist_x(std::move(hist_x)),
            hist_y(std::move(hist_y)),
            dmg_sources(std::move(dmg_sources)),
            time_lapse_names(std::move(time_lapse_names)),
            damage_time_lapse(std::move(damage_time_lapse)),
            aura_uptimes(std::move(aura_uptimes)),
            use_effect_order_string(std::move(use_effect_order_string)),
            proc_counter(std::move(proc_counter)),
            stat_weights(std::move(stat_weights)),
            extra_stats(std::move(extra_stats)),
            mean_dps(std::move(mean_dps)),
            std_dps(std::move(std_dps)),
            messages(std::move(messages)) {}

    std::vector<int> hist_x;
    std::vector<int> hist_y;
    std::vector<double> dmg_sources;
    std::vector<std::string> time_lapse_names;
    std::vector<std::vector<double>> damage_time_lapse;
    std::vector<std::string> aura_uptimes;
    std::vector<std::string> use_effect_order_string;
    std::vector<std::string> proc_counter;
    std::vector<std::string> stat_weights;
    std::vector<std::string> extra_stats;
    std::vector<double> mean_dps{};
    std::vector<double> std_dps{};
    std::vector<std::string> messages;
};

#endif // COVID_OUTPUT_HPP
