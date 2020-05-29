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
              double fight_time,
              double target_level,
              double n_simulations,
              int sunder_armor,
              bool faerie_fire,
              bool curse_of_recklessness,
              bool death_wish,
              bool recklessness,
              bool mighty_rage_potion,
              bool vaelastrasz,
              bool debug_on,
              bool use_bt_in_exec_phase,
              bool use_hs_in_exec_phase,
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
            fight_time(fight_time),
            target_level(target_level),
            n_simulations(n_simulations),
            sunder_armor(sunder_armor),
            faerie_fire(faerie_fire),
            curse_of_recklessness(curse_of_recklessness),
            death_wish(death_wish),
            recklessness(recklessness),
            mighty_rage_potion(mighty_rage_potion),
            vaelastrasz(vaelastrasz),
            debug_on(debug_on),
            use_bt_in_exec_phase(use_bt_in_exec_phase),
            use_hs_in_exec_phase(use_hs_in_exec_phase),
            heroic_strike_rage_thresh(heroic_strike_rage_thresh),
            whirlwind_rage_thresh(whirlwind_rage_thresh),
            whirlwind_bt_cooldown_thresh(whirlwind_bt_cooldown_thresh) {}

    std::vector<std::string> race;
    std::vector<std::string> armor;
    std::vector<std::string> weapons;
    std::vector<std::string> buffs;
    std::vector<std::string> enchants;
    std::vector<std::string> stat_weights;
    double fight_time{};
    double target_level{};
    double n_simulations{};
    double sunder_armor{};
    bool faerie_fire{};
    bool curse_of_recklessness{};
    bool death_wish{};
    bool recklessness{};
    bool mighty_rage_potion{};
    bool vaelastrasz{};
    bool debug_on{};
    bool use_bt_in_exec_phase{};
    bool use_hs_in_exec_phase{};
    double heroic_strike_rage_thresh{};
    double whirlwind_rage_thresh{};
    double whirlwind_bt_cooldown_thresh{};
};

#endif // COVID_INPUT_HPP
