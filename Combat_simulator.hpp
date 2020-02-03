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

    enum class Hit_type
    {
        white,
        yellow
    };

    struct Hit_outcome
    {
        Hit_outcome(double damage, Hit_result hit_type) : damage{damage}, hit_type{hit_type} {};

        double damage;
        Hit_result hit_type;
    };

    double simulate(const Character &character, double sim_time, double dt, int level);

    Combat_simulator::Hit_outcome generate_hit(double damage, Hit_type hit_type);

    void compute_hit_table(double level, double weapon_skill, Special_stats special_stats);

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
};

#endif //WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
