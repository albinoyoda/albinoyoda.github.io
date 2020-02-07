#ifndef WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
#define WOW_SIMULATOR_COMBAT_SIMULATOR_HPP

#include <vector>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <map>
#include <cmath>

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

    struct Damage_sources
    {
        Damage_sources() = default;

        Damage_sources &operator+(const Damage_sources &rhs)
        {
            this->whirlwind = this->whirlwind + rhs.whirlwind;
            this->bloodthirst = this->bloodthirst + rhs.bloodthirst;
            this->execute = this->execute + rhs.execute;
            this->white_mh = this->white_mh + rhs.white_mh;
            this->white_oh = this->white_oh + rhs.white_oh;
            this->extra_hit = this->extra_hit + rhs.extra_hit;
            this->heroic_strike = this->heroic_strike + rhs.heroic_strike;
            return *(this);
        }


        constexpr double sum() const
        {
            return white_mh + white_oh + extra_hit + bloodthirst + heroic_strike + whirlwind + execute;
        }

        double white_mh;
        double white_oh;
        double extra_hit;
        double bloodthirst;
        double execute;
        double heroic_strike;
        double whirlwind;
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

    std::vector<double> &
    simulate(const Character &character, double sim_time, int opponent_level, int n_damage_batches);

    template<typename Struct_t, typename Field_t>
    Stat_weight permute_stat(const Character &character, Struct_t struct_t, Field_t field_t, Stat stat, double amount,
                             double sim_time, int opponent_level, int n_batches, double mean_init,
                             double sample_std_init);

    std::vector<Combat_simulator::Stat_weight>
    compute_stat_weights(const Character &character, double sim_time, int opponent_level, int n_batches,
                         double mean_init, double sample_std_init);

    Combat_simulator::Hit_outcome generate_hit(double damage, Hit_type hit_type, Hand weapon_hand, bool death_wish);

    Combat_simulator::Hit_outcome generate_hit_oh(double damage);

    Combat_simulator::Hit_outcome generate_hit_mh(double damage, Hit_type hit_type);

    void compute_hit_table(int opponent_level, int weapon_skill, Special_stats special_stats, Hand weapon_hand);

    void compute_hit_table_oh_(int opponent_level, int weapon_skill, Special_stats special_stats);

    void compute_hit_table_mh_(int opponent_level, int weapon_skill, Special_stats special_stats);

    void enable_spell_rotation();

    void enable_talents();

    void enable_item_chance_on_hit_effects();

    void enable_crusader();

    void enable_death_wish();

    void enable_recklessness();

    static double average(const std::vector<double> &vec);

    static double standard_deviation(const std::vector<double> &vec, double ave);

    static double variance(const std::vector<double> &vec, double ave);

    static double sample_deviation(double mean, int n_samples);

    static double add_standard_deviations(double std1, double std2);

    const std::vector<double> &get_hit_probabilities_white_mh() const;

    const std::vector<Damage_sources> &get_damage_distribution() const;

    template<typename T>
    double damage_source_std(T field_ptr) const
    {
        std::vector<double> damage_vec;
        damage_vec.reserve(damage_distribution_.size());
        for (const auto &damage_source : damage_distribution_)
        {
            damage_vec.push_back(damage_source.*field_ptr / damage_source.sum());
        }
        double mean_dps = Combat_simulator::average(damage_vec);
        double std_dps = Combat_simulator::standard_deviation(damage_vec, mean_dps);
//        double sample_std_dps = Combat_simulator::sample_deviation(std_dps, damage_vec.size());
        return std_dps;
    }

    void print_damage_distribution() const;

private:
    std::vector<double> hit_probabilities_white_mh_;
    std::vector<double> hit_probabilities_white_oh_;
    std::vector<double> hit_probabilities_yellow_;
    std::vector<double> batch_damage_{};
    std::vector<Damage_sources> damage_distribution_{};
    bool spell_rotation_{false};
    bool item_chance_on_hit_{false};
    bool talents_{false};
    bool crusader_enabled_{false};
    bool death_wish_enabled_{false};
    bool recklessness_enabled_{false};
    double glancing_factor_mh_{0.0};
    double glancing_factor_oh_{0.0};
    double armor_reduction_factor_{};
};

std::ostream &operator<<(std::ostream &os, Combat_simulator::Stat_weight const &stats);

#include "Combat_simulator.tcc"

#endif //WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
