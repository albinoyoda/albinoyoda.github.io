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
    enum class Hit_type
    {
        miss,
        dodge,
        glancing,
        crit,
        hit
    };

    struct Outcome
    {
        Outcome() = delete;

        Outcome(double damage, Hit_type hit_type) : damage{damage}, hit_type{hit_type} {};

        double damage;
        Hit_type hit_type;
        bool did_heroic_strike;
    };

    double simulate(const Character &character, double sim_time, double dt, int level);

    Combat_simulator::Outcome generate_hit(double damage);

    void compute_hit_table(double level, double weapon_skill, Special_stats special_stats);

    void enable_spell_rotation();

    void enable_talents();

    void enable_item_change_on_hit_effects();

private:
    std::vector<double> hit_probabilities_;
    std::vector<double> damage_weights_;
    bool spell_rotation_;
    bool item_chance_on_hit_;
    bool talents_;
};

#endif //WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
