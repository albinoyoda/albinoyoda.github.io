#ifndef WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
#define WOW_SIMULATOR_COMBAT_SIMULATOR_HPP

#include <vector>
#include <iostream>
#include <cassert>
#include <map>
#include <cmath>
#include <iomanip>
#include <random>

#include "Attributes.hpp"
#include "Character.hpp"
#include "time_keeper.hpp"

class Time_keeper
{
public:
    Time_keeper() = default;

    void increment(double dt)
    {
        blood_thirst_cd -= dt;
        whirlwind_cd -= dt;
        global_cd -= dt;
        crusader_mh_buff_timer -= dt;
        crusader_oh_buff_timer -= dt;
        time += dt;
        step_index++;
    }

    void reset()
    {
        blood_thirst_cd = -1e-10;
        whirlwind_cd = -1e-10;
        global_cd = -1e-10;
        crusader_mh_buff_timer = -1e-10;
        crusader_oh_buff_timer = -1e-10;
        time = 0.0;
        step_index = 1;
    }

    constexpr double get_dynamic_time_step(double mh_dt,
                                           double oh_dt)
    {
        double dt = 100.0;
        if (blood_thirst_cd > 0.0)
        {
            dt = std::min(blood_thirst_cd, dt);
        }
        if (whirlwind_cd > 0.0)
        {
            dt = std::min(whirlwind_cd, dt);
        }
        if (global_cd > 0.0)
        {
            dt = std::min(global_cd, dt);
        }
        if (crusader_mh_buff_timer > 0.0)
        {
            dt = std::min(crusader_mh_buff_timer, dt);
        }
        if (crusader_oh_buff_timer > 0.0)
        {
            dt = std::min(crusader_oh_buff_timer, dt);
        }
        dt = std::min(mh_dt, dt);
        dt = std::min(oh_dt, dt);
        dt += 1e-5;
        dt = dt;
        return dt;
    }

    double blood_thirst_cd;
    double whirlwind_cd;
    double global_cd;
    double crusader_mh_buff_timer;
    double crusader_oh_buff_timer;
    double time;
    double dt;
    int step_index;
};

struct Combat_simulator_config
{
    Combat_simulator_config() = default;

    bool spell_rotation;
    bool heroic_strike_spamm;
    bool item_chance_on_hit;
    bool talents;
    bool crusader_enabled;
    bool death_wish_enabled;
    bool recklessness_enabled;
    bool bloodrage_enabled;
    bool debug_mode;
    bool use_fast_rng;
    bool random_melee_hits;
    bool use_mighty_rage_potion;
    bool anger_management_enabled;
    bool fuel_extra_rage;
    bool seed;
    double sim_time;
    int target_level;
    int n_damage_batches;


    int talent_improved_heroic_strike;
    int talent_flurry;


    bool armor_reduction_from_spells;
    bool sunder_armor;
    bool cor;
    bool faerie_fire;


};

class Combat_simulator
{
public:
    explicit Combat_simulator(Combat_simulator_config config) : config(config) {};

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
        Hit_outcome(double damage, Hit_result hit_result) : damage{damage}, hit_result{hit_result} {};

        double damage;
        Hit_result hit_result;
    };

    struct Stat_weight
    {
        Stat_weight(Stat stat) : stat(stat)
        {
            d_dps_plus = 0.0;
            std_d_dps_plus = 0.0;
            d_dps_minus = 0.0;
            std_d_dps_minus = 0.0;
            amount = 0.0;
        };

        Stat_weight(double d_dps_plus, double std_d_dps_plus, double d_dps_minus, double std_d_dps_minus, double amount,
                    Stat stat) :
                d_dps_plus{d_dps_plus},
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

    std::vector<double> &simulate(const Character &character);

    std::vector<Combat_simulator::Stat_weight>
    compute_stat_weights(const Character &character, const Armory &armory, double sim_time, int opponent_level,
                         int n_batches,
                         double mean_init, double sample_std_init);

    Combat_simulator::Hit_outcome
    generate_hit(double damage, Hit_type hit_type, Hand weapon_hand, bool heroic_strike_active, bool death_wish,
                 bool recklessness_active);

    Combat_simulator::Hit_outcome generate_hit_oh(double damage, bool heroic_strike_active, bool recklessness_active);

    Combat_simulator::Hit_outcome
    generate_hit_mh(double damage, Hit_type hit_type, bool recklessness_active);

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

    void fuel_extra_rage(double interval, double damage_amount)
    {
        fuel_extra_rage_ = true;
        interval_ = interval;
        damage_amount_ = damage_amount;
    }

    template<typename T>
    void print_statement(T t)
    {
        if (config.debug_mode)
        {
            std::cout << std::setprecision(4) << t;
        }
    }

    template<typename... Args>
    void simulator_cout(Args &&... args)
    {
        if (config.debug_mode)
        {
            std::cout << "Time: " << std::setw(8) << std::left << time_keeper_.time + time_keeper_.dt
                      << "s. Loop idx:" << std::setw(4) << time_keeper_.step_index << "Event: ";
            __attribute__((unused)) int dummy[] = {0, ((void) print_statement(std::forward<Args>(args)), 0)...};
            std::cout << "\n";
        }
    }

private:
    std::vector<double> batch_damage_{};
    Combat_simulator_config config;
    double interval_;
    double damage_amount_;
    double armor_reduction_factor_{};
    Time_keeper time_keeper_{};
};

std::ostream &operator<<(std::ostream &os, Combat_simulator::Stat_weight const &stats);

#include "Combat_simulator.tcc"

#endif //WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
