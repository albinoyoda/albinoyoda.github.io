#ifndef SIM_INPUT_MULT_HPP
#define SIM_INPUT_MULT_HPP

#include <vector>

struct Sim_input_mult
{
    Sim_input_mult() = default;

    Sim_input_mult(std::vector<std::string> race,
                   std::vector<std::string> armor,
                   std::vector<std::string> weapons,
                   std::vector<std::string> buffs,
                   std::vector<std::string> enchants,
                   std::vector<std::string> options,
                   double fight_time,
                   double target_level,
                   int sunder_armor,
                   double heroic_strike_rage_thresh,
                   double cleave_rage_thresh,
                   double whirlwind_rage_thresh,
                   double whirlwind_bt_cooldown_thresh,
                   double hamstring_cd_thresh,
                   double hamstring_thresh_dd,
                   double initial_rage,
                   double max_optimize_time
                  ) :
            race(std::move(race)),
            armor(std::move(armor)),
            weapons(std::move(weapons)),
            buffs(std::move(buffs)),
            enchants(std::move(enchants)),
            options(std::move(options)),
            fight_time(fight_time),
            target_level(target_level),
            sunder_armor(sunder_armor),
            heroic_strike_rage_thresh(heroic_strike_rage_thresh),
            cleave_rage_thresh(cleave_rage_thresh),
            whirlwind_rage_thresh(whirlwind_rage_thresh),
            whirlwind_bt_cooldown_thresh(whirlwind_bt_cooldown_thresh),
            hamstring_cd_thresh(hamstring_cd_thresh),
            hamstring_thresh_dd(hamstring_thresh_dd),
            initial_rage(initial_rage),
            max_optimize_time(max_optimize_time) {}

    std::vector<std::string> race;
    std::vector<std::string> armor;
    std::vector<std::string> weapons;
    std::vector<std::string> buffs;
    std::vector<std::string> enchants;
    std::vector<std::string> options;
    double fight_time{};
    double target_level{};
    double sunder_armor{};
    double heroic_strike_rage_thresh{};
    double cleave_rage_thresh{};
    double whirlwind_rage_thresh{};
    double whirlwind_bt_cooldown_thresh{};
    double hamstring_cd_thresh{};
    double hamstring_thresh_dd{};
    double initial_rage{};
    double max_optimize_time{};
};


#endif // SIM_INPUT_MULT_HPP
