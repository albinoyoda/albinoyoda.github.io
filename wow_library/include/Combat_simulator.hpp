#ifndef WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
#define WOW_SIMULATOR_COMBAT_SIMULATOR_HPP

#include <vector>
#include <iostream>
#include <cassert>
#include <map>
#include <cmath>
#include <iomanip>
#include <random>

#include "Character.hpp"
#include "damage_sources.hpp"
#include "time_keeper.hpp"
#include "weapon_sim.hpp"
#include "Buff_manager.hpp"
#include "Statistics.hpp"

struct Combat_simulator_config
{
    // Combat settings
    int n_batches{};
    double sim_time{};
    int opponent_level{};

    // Simulator settings
    bool enable_rng_melee{false};
    bool enable_spell_rotation{false};
    bool use_heroic_spamm{false};
    bool use_mighty_rage_potion{false};
    bool enable_anger_management{false};
    bool enable_bloodrage{false};
    bool enable_talents{false};
    bool enable_item_chance_on_hit_effects{false};
    bool enable_crusader{false};
    bool enable_death_wish{false};
    bool enable_recklessness{false};
    bool display_combat_debug{false};
    bool use_seed{false};
    int seed{};
    bool fuel_extra_rage{false};
    int extra_rage_interval{};
    int extra_rage_damage_amount{};
};

class Combat_simulator
{
public:
    explicit Combat_simulator(Combat_simulator_config config) : config_(config)
    {
        if (config_.use_seed)
        {
            srand(config_.seed);
        }
    }

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
        skill_sword,
        skill_axe,
        skill_mace,
        skill_dagger,
        weapon_damage,
        NONE
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
        Stat_weight(Stat stat) : stat(stat)
        {
            d_dps_plus = 0.0;
            std_d_dps_plus = 0.0;
            d_dps_minus = 0.0;
            std_d_dps_minus = 0.0;
            amount = 0.0;
        };

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

    constexpr double lookup_outcome_mh_white(int case_id)
    {
        switch (case_id)
        {
            case 0:
                return 0.0;
            case 1:
                return 0.0;
            case 2:
                return glancing_factor_mh_;
            case 3:
                return 2.2;
            case 4:
                return 1.0;
            default:
                assert(false);
                return 0.0;
        }
    }

    constexpr double lookup_outcome_oh(int case_id)
    {
        switch (case_id)
        {
            case 0:
                return 0.0;
            case 1:
                return 0.0;
            case 2:
                return glancing_factor_oh_;
            case 3:
                return 2.2;
            case 4:
                return 1.0;
            default:
                assert(false);
                return 0.0;
        }
    }

    void swing_weapon(Weapon_sim &weapon, Weapon_sim &main_hand_weapon, Special_stats special_stats,
                      bool &heroic_strike_,
                      double &rage, double &heroic_strike_rage_cost, bool &deathwish_active,
                      bool &recklessness_active, Damage_sources &damage_sources, int &flurry_charges,
                      double &flurry_dt_factor, double &flurry_speed_bonus);

    std::vector<double> &simulate(const Character &character);

    double get_uniform_random(double r_max)
    {
        return rand() * r_max / RAND_MAX;
    }

    //
//    template<typename Struct_t, typename Field_t>
//    Stat_weight permute_stat(const Character &character, const Armory& armory, Struct_t struct_t, Field_t field_t, Stat stat, double amount,
//                             double sim_time, int opponent_level, int n_batches, double mean_init,
//                             double sample_std_init);
//
//    template<typename Function_ptr>
//    Combat_simulator::Stat_weight
//    permute_stat(const Character &character,  const Armory& armory, Function_ptr function_ptr,
//                 Combat_simulator::Stat stat, double amount, double sim_time, int opponent_level,
//                 int n_batches, double mean_init, double sample_std_init);

//    std::vector<Combat_simulator::Stat_weight>
//    compute_stat_weights(const Character &character, const Armory &armory,double sim_time, int opponent_level, int n_batches,
//                         double mean_init, double sample_std_init);

    Combat_simulator::Hit_outcome
    generate_hit(double damage, Hit_type hit_type, Socket weapon_hand, bool heroic_strike_active, bool death_wish,
                 bool recklessness_active);

    Combat_simulator::Hit_outcome generate_hit_oh(double damage, bool heroic_strike_active, bool recklessness_active);

    Combat_simulator::Hit_outcome generate_hit_mh(double damage, Hit_type hit_type, bool recklessness_active);

    void compute_hit_table(int level_difference, int weapon_skill, Special_stats special_stats, Socket weapon_hand);



    const std::vector<double> &get_hit_probabilities_white_mh() const;

    template<typename T>
    double damage_source_std(T field_ptr) const
    {
        std::vector<double> damage_vec;
        damage_vec.reserve(damage_distribution_.size());
        for (const auto &damage_source : damage_distribution_)
        {
            damage_vec.push_back(damage_source.*field_ptr / damage_source.sum_counts());
        }
        double mean_dps = Statistics::average(damage_vec);
        double std_dps = Statistics::standard_deviation(damage_vec, mean_dps);
        return std_dps;
    }

    void print_damage_distribution() const;

    void print_damage_sources(const std::string &source_name, double source_percent,
                              double source_std, double source_count) const
    {
        if (source_count > 0)
        {
            std::cout << source_name << std::setw(5) << std::left << std::setprecision(3)
                      << 100 * source_percent << " +- " << std::setw(4) << 100 * source_std << ", casts: "
                      << source_count << "\n";
        }
    }

    template<typename T>
    void print_statement(T t)
    {
        std::cout << std::setprecision(4) << t;
    }

    template<typename... Args>
    void simulator_cout(Args &&... args)
    {
        if (config_.display_combat_debug)
        {
            std::cout << "Time: " << std::setw(8) << std::left << time_keeper_.time + time_keeper_.current_dt
                      << "s. Loop idx:" << std::setw(4) << time_keeper_.step_index << "Event: ";
            __attribute__((unused)) int dummy[] = {0, ((void) print_statement(std::forward<Args>(args)), 0)...};
            std::cout << "\n";
        }
    }

private:
    std::vector<double> hit_probabilities_white_mh_;
    std::vector<double> hit_probabilities_white_oh_;
    std::vector<double> hit_probabilities_yellow_;
    std::vector<double> hit_probabilities_recklessness_mh_;
    std::vector<double> hit_probabilities_recklessness_oh_;
    std::vector<double> hit_probabilities_recklessness_yellow_;
    std::vector<double> hit_probabilities_two_hand_;
    std::vector<double> hit_probabilities_recklessness_two_hand_;
    std::vector<double> batch_damage_{};
    std::vector<Damage_sources> damage_distribution_{};
    double glancing_factor_mh_{};
    double glancing_factor_oh_{};
    double armor_reduction_factor_{};
    Time_keeper time_keeper_{};
    Buff_manager buff_manager_{};
    Combat_simulator_config config_;
};

std::ostream &operator<<(std::ostream &os, Combat_simulator::Stat_weight const &stats);

#include "Combat_simulator.tcc"

#endif //WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
