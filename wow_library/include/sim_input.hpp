#ifndef COVID_INPUT_HPP
#define COVID_INPUT_HPP

#include <vector>

struct Sim_input
{
    Sim_input() = default;

    Sim_input(std::vector<std::string> race,
              std::vector<std::string> armor,
              std::vector<std::string> weapons,
              std::vector<std::string> buffs,
              std::vector<std::string> enchants,
              std::vector<std::string> stat_weights,
              std::vector<std::string> options,
              double fight_time,
              double target_level,
              double n_simulations,
              int sunder_armor,
              double heroic_strike_rage_thresh,
              double whirlwind_rage_thresh,
              double whirlwind_bt_cooldown_thresh
             ) :
            race(std::move(race)),
            armor(std::move(armor)),
            weapons(std::move(weapons)),
            buffs(std::move(buffs)),
            enchants(std::move(enchants)),
            stat_weights(std::move(stat_weights)),
            options(std::move(options)),
            fight_time(fight_time),
            target_level(target_level),
            n_simulations(n_simulations),
            sunder_armor(sunder_armor),
            heroic_strike_rage_thresh(heroic_strike_rage_thresh),
            whirlwind_rage_thresh(whirlwind_rage_thresh),
            whirlwind_bt_cooldown_thresh(whirlwind_bt_cooldown_thresh) {}

    std::vector<std::string> race;
    std::vector<std::string> armor;
    std::vector<std::string> weapons;
    std::vector<std::string> buffs;
    std::vector<std::string> enchants;
    std::vector<std::string> stat_weights;
    std::vector<std::string> options;
    double fight_time{};
    double target_level{};
    double n_simulations{};
    double sunder_armor{};
    double heroic_strike_rage_thresh{};
    double whirlwind_rage_thresh{};
    double whirlwind_bt_cooldown_thresh{};
};

#endif // COVID_INPUT_HPP
