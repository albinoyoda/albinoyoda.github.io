#ifndef SIM_INPUT_HPP
#define SIM_INPUT_HPP

#include <vector>
#include <string>

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
              std::vector<std::string> compare_armor,
              std::vector<std::string> compare_weapons,
              double fight_time,
              double target_level,
              double n_simulations,
              double n_simulations_stat_weights,
              int sunder_armor,
              double heroic_strike_rage_thresh,
              double cleave_rage_thresh,
              double whirlwind_rage_thresh,
              double whirlwind_bt_cooldown_thresh,
              double hamstring_cd_thresh,
              double hamstring_thresh_dd,
              double initial_rage
             ) :
            race(std::move(race)),
            armor(std::move(armor)),
            weapons(std::move(weapons)),
            buffs(std::move(buffs)),
            enchants(std::move(enchants)),
            stat_weights(std::move(stat_weights)),
            options(std::move(options)),
            compare_armor(std::move(compare_armor)),
            compare_weapons(std::move(compare_weapons)),
            fight_time(fight_time),
            target_level(target_level),
            n_simulations(n_simulations),
            n_simulations_stat_weights(n_simulations_stat_weights),
            sunder_armor(sunder_armor),
            heroic_strike_rage_thresh(heroic_strike_rage_thresh),
            cleave_rage_thresh(cleave_rage_thresh),
            whirlwind_rage_thresh(whirlwind_rage_thresh),
            whirlwind_bt_cooldown_thresh(whirlwind_bt_cooldown_thresh),
            hamstring_cd_thresh(hamstring_cd_thresh),
            hamstring_thresh_dd(hamstring_thresh_dd),
            initial_rage(initial_rage) {}

    std::vector<std::string> race;
    std::vector<std::string> armor;
    std::vector<std::string> weapons;
    std::vector<std::string> buffs;
    std::vector<std::string> enchants;
    std::vector<std::string> stat_weights;
    std::vector<std::string> options;
    std::vector<std::string> compare_armor;
    std::vector<std::string> compare_weapons;
    double fight_time{};
    double target_level{};
    double n_simulations{};
    double n_simulations_stat_weights{};
    double sunder_armor{};
    double heroic_strike_rage_thresh{};
    double cleave_rage_thresh{};
    double whirlwind_rage_thresh{};
    double whirlwind_bt_cooldown_thresh{};
    double hamstring_cd_thresh{};
    double hamstring_thresh_dd{};
    double initial_rage{};
};

#endif // SIM_INPUT_HPP
