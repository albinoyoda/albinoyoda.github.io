#ifndef COVID_OUTPUT_HPP
#define COVID_OUTPUT_HPP

#include <utility>

struct Sim_output
{
    Sim_output() = default;

    Sim_output(
            std::vector<double> hist_x,
            std::vector<int> hist_y,
            std::vector<double> dmg_sources,
            std::vector<double> white_mh,
            std::vector<double> white_oh,
            std::vector<double> bloodthirst,
            std::vector<double> execute,
            std::vector<double> heroic_strike,
            std::vector<double> cleave,
            std::vector<double> whirlwind,
            std::vector<double> hamstring,
            std::vector<double> item_hit_effects,
            std::vector<std::string> aura_uptimes,
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
            white_mh(std::move(white_mh)),
            white_oh(std::move(white_oh)),
            bloodthirst(std::move(bloodthirst)),
            execute(std::move(execute)),
            heroic_strike(std::move(heroic_strike)),
            cleave(std::move(cleave)),
            whirlwind(std::move(whirlwind)),
            hamstring(std::move(hamstring)),
            item_hit_effects(std::move(item_hit_effects)),
            aura_uptimes(std::move(aura_uptimes)),
            proc_counter(std::move(proc_counter)),
            stat_weights(std::move(stat_weights)),
            extra_stats(std::move(extra_stats)),
            mean_dps(std::move(mean_dps)),
            std_dps(std::move(std_dps)),
            messages(std::move(messages)) {}

    std::vector<double> hist_x;
    std::vector<int> hist_y;
    std::vector<double> dmg_sources;
    std::vector<double> white_mh;
    std::vector<double> white_oh;
    std::vector<double> bloodthirst;
    std::vector<double> execute;
    std::vector<double> heroic_strike;
    std::vector<double> cleave;
    std::vector<double> whirlwind;
    std::vector<double> hamstring;
    std::vector<double> item_hit_effects;
    std::vector<std::string> aura_uptimes;
    std::vector<std::string> proc_counter;
    std::vector<std::string> stat_weights;
    std::vector<std::string> extra_stats;
    std::vector<double> mean_dps{};
    std::vector<double> std_dps{};
    std::vector<std::string> messages;
};

#endif // COVID_OUTPUT_HPP
