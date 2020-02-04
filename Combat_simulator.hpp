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
        Stat_weight(double d_dps_plus, double std_d_dps_plus, double d_dps_minus, double std_d_dps_minus, double amount,
                    Stat stat) : d_dps_plus{d_dps_plus},
                                 std_d_dps_plus{std_d_dps_plus},
                                 d_dps_minus{d_dps_minus},
                                 std_d_dps_minus{std_d_dps_minus},
                                 amount{amount},
                                 stat{stat} {};
        double d_dps_plus;
        double std_d_dps_plus;
        double d_dps_minus;
        double std_d_dps_minus;
        double amount;
        Stat stat;
    };

    std::vector<double>
    simulate(const Character &character, double sim_time, int opponent_level, int n_damage_batches);

    template<typename Struct_t, typename Field_t>
    Stat_weight permute_stat(const Character &character, Struct_t struct_t, Field_t field_t, Stat stat, double amount,
                             double sim_time,
                             int opponent_level, int n_batches);

    std::vector<Combat_simulator::Stat_weight>
    compute_stat_weights(const Character &character, double sim_time, int opponent_level, int n_batches);

    Combat_simulator::Hit_outcome generate_hit(double damage, Hit_type hit_type);

    void compute_hit_table(int opponent_level, int weapon_skill, Special_stats special_stats);

    void enable_spell_rotation();

    void enable_talents();

    void enable_item_chance_on_hit_effects();

    static double average(const std::vector<double> &vec);

    static double standard_deviation(const std::vector<double> &vec, double ave);

    static double variance(const std::vector<double> &vec, double ave);

    static double sample_deviation(double mean, int n_samples);

    static double add_standard_deviations(double std1, double std2);

    void enable_crusader();

private:
    std::vector<double> hit_probabilities_white_;
    std::vector<double> hit_probabilities_yellow_;
//    std::vector<double> damage_weights_;
    bool spell_rotation_{false};
    bool item_chance_on_hit_{false};
    bool talents_{false};
    bool crusader_enabled_{false};
    double glancing_factor_{0.0};
};

std::ostream &operator<<(std::ostream &os, Combat_simulator::Stat_weight const &stats);

#include "Combat_simulator.tcc"

#endif //WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
