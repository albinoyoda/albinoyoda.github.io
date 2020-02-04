#ifndef WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
#define WOW_SIMULATOR_COMBAT_SIMULATOR_HPP

#include <vector>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include "Stats.hpp"
#include "Character.hpp"

class Combat_simulator
{
public:
    enum class Hit_result
    {
        miss,
        dodge,
        glancing,
        crit,
        hit,
        TBD
    };

    enum class Stat
    {
        agility,
        strength,
        crit,
        hit,
        chance_extra_hit,
        haste,
        skill,
        // TODO add more
    };

    enum class Hit_type
    {
        white,
        yellow
    };

    struct Hit_outcome
    {
        Hit_outcome(double damage, Hit_result hit_result) : damage{damage}, hit_result{hit_result} {};

        double damage;
        Hit_result hit_result;
    };

    struct Stat_weight
    {
        double d_dps;
        Stat stat;
    };


    std::vector<double> simulate(const Character &character, double sim_time, double dt, int opponent_level);

    std::vector<double>
    compute_stat_weights(const Character &character, double sim_time, double dt, int opponent_level);

    Combat_simulator::Hit_outcome generate_hit(double damage, Hit_type hit_type);

    void compute_hit_table(int opponent_level, int weapon_skill, Special_stats special_stats);

    void enable_spell_rotation();

    void enable_talents();

    void enable_item_change_on_hit_effects();

private:
    std::vector<double> hit_probabilities_white_;
    std::vector<double> hit_probabilities_yellow_;
    std::vector<double> damage_weights_;
    bool spell_rotation_{false};
    bool item_chance_on_hit_{false};
    bool talents_{false};
    double glancing_factor_{0.0};
};

#endif //WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
