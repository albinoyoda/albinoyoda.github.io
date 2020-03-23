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
        time_ += dt;
        step_index_++;
    }

    void reset()
    {
        blood_thirst_cd = -1e-10;
        whirlwind_cd = -1e-10;
        global_cd = -1e-10;
        crusader_mh_buff_timer = -1e-10;
        crusader_oh_buff_timer = -1e-10;
        time_ = 0.0;
        step_index_ = 1;
    }

    constexpr double get_dynamic_time_step(double mh_dt,
                                           double oh_dt,
                                           double sim_dt)
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
        dt = std::min(sim_dt, dt);
        dt += 1e-5;
        dt_ = dt;
        return dt;
    }

    double blood_thirst_cd;
    double whirlwind_cd;
    double global_cd;
    double crusader_mh_buff_timer;
    double crusader_oh_buff_timer;
    double time_;
    double dt_;
    int step_index_;
};

class Combat_simulator
{
public:
    explicit Combat_simulator() : eng_{static_cast<long unsigned int>(time(nullptr))},
            dist100_{0.0, 100.0}, dist1_(0.0, 1.0) {}

    void set_seed(long unsigned int seed)
    {
        if (use_fast_rng_)
        {
            srand(seed);
        }
        else
        {
            eng_ = std::default_random_engine{seed};
        }

    }

    void use_heroic_spamm()
    {
        heroic_strike_spamm_ = true;
    }

    void use_fast_but_sloppy_rng()
    {
        use_fast_rng_ = true;
    }

    double get_random_100()
    {
        if (use_fast_rng_)
        {
            return rand() * 100.0 / RAND_MAX;
        }
        else
        {
            return dist100_(eng_);
        }
    }

    double get_random_1()
    {
        if (use_fast_rng_)
        {
            return rand() * 1.0 / RAND_MAX;
        }
        else
        {
            return dist1_(eng_);
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
            this->whirlwind_count = this->whirlwind_count + rhs.whirlwind_count;
            this->bloodthirst_count = this->bloodthirst_count + rhs.bloodthirst_count;
            this->execute_count = this->execute_count + rhs.execute_count;
            this->white_mh_count = this->white_mh_count + rhs.white_mh_count;
            this->white_oh_count = this->white_oh_count + rhs.white_oh_count;
            this->extra_hit_count = this->extra_hit_count + rhs.extra_hit_count;
            this->heroic_strike_count = this->heroic_strike_count + rhs.heroic_strike_count;
            return *(this);
        }

        constexpr double sum() const
        {
            return white_mh + white_oh + extra_hit + bloodthirst + heroic_strike + whirlwind + execute;
        }

        constexpr double sum_counts() const
        {
            return white_mh_count + white_oh_count + extra_hit_count + bloodthirst_count + heroic_strike_count + whirlwind_count + execute_count;
        }

        double white_mh;
        double white_oh;
        double extra_hit;
        double bloodthirst;
        double execute;
        double heroic_strike;
        double whirlwind;
        size_t white_mh_count;
        size_t white_oh_count;
        size_t extra_hit_count;
        size_t bloodthirst_count;
        size_t execute_count;
        size_t heroic_strike_count;
        size_t whirlwind_count;
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

    static constexpr double lookup_outcome_mh_yellow(int case_id)
    {
        switch (case_id)
        {
            case 0:
                return 0.0;
            case 1:
                return 0.0;
            case 2:
                return 2.2;
            case 3:
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

    std::vector<double> &
    simulate(const Character &character, double sim_time, int opponent_level, int n_damage_batches);

    template<typename Struct_t, typename Field_t>
    Stat_weight permute_stat(const Character &character, const Armory& armory, Struct_t struct_t, Field_t field_t, Stat stat, double amount,
                             double sim_time, int opponent_level, int n_batches, double mean_init,
                             double sample_std_init);

    template<typename Function_ptr>
    Combat_simulator::Stat_weight
    permute_stat(const Character &character,  const Armory& armory, Function_ptr function_ptr,
                 Combat_simulator::Stat stat, double amount, double sim_time, int opponent_level,
                 int n_batches, double mean_init, double sample_std_init);

    std::vector<Combat_simulator::Stat_weight>
    compute_stat_weights(const Character &character, const Armory &armory,double sim_time, int opponent_level, int n_batches,
                         double mean_init, double sample_std_init);

    Combat_simulator::Hit_outcome
    generate_hit(double damage, Hit_type hit_type, Hand weapon_hand, bool heroic_strike_active, bool death_wish,
                 bool recklessness_active);

    Combat_simulator::Hit_outcome generate_hit_oh(double damage, bool heroic_strike_active, bool recklessness_active);

    Combat_simulator::Hit_outcome
    generate_hit_mh(double damage, Hit_type hit_type, bool recklessness_active);

    void compute_hit_table(int level_difference, int weapon_skill, Special_stats special_stats, Hand weapon_hand);

    void compute_hit_table_oh_(int level_difference, int weapon_skill, Special_stats special_stats);

    void compute_hit_table_mh_(int level_difference, int weapon_skill, Special_stats special_stats);

    void enable_spell_rotation();

    void enable_talents();

    void enable_item_chance_on_hit_effects();

    void enable_crusader();

    void enable_death_wish();

    void enable_recklessness();

    void enable_bloodrage();

    void use_mighty_rage_potion();

    void enable_anger_management();

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

    void display_combat_debug()
    {
        debug_mode_ = true;
    }

    void enable_rng_melee()
    {
        random_melee_hits_ = true;
    }

    void fuel_extra_rage(double interval, double damage_amount)
    {
        fuel_extra_rage_ = true;
        interval_ = interval;
        damage_amount_ = damage_amount;
    }

    void print_damage_sources(const std::string &source_name, double source_percent,
                              double source_std, double source_count) const
    {
        std::cout << source_name << std::setw(5) << std::left << std::setprecision(3)
                  << 100 * source_percent << " +- " << std::setw(4) << 100 * source_std << ", casts: "
                  << source_count << "\n";
    }

    template<typename T>
    void print_statement(T t)
    {
        if (debug_mode_)
        {
            std::cout << std::setprecision(4) << t;
        }
    }

    template<typename... Args>
    void simulator_cout(Args &&... args)
    {
        if (debug_mode_)
        {
            std::cout << "Time: " << std::setw(8) << std::left << time_keeper_.time_ + time_keeper_.dt_
                      << "s. Loop idx:" << std::setw(4) << time_keeper_.step_index_ << "Event: ";
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
    bool spell_rotation_{false};
    bool heroic_strike_spamm_{false};
    bool item_chance_on_hit_{false};
    bool talents_{false};
    bool crusader_enabled_{false};
    bool death_wish_enabled_{false};
    bool recklessness_enabled_{false};
    bool bloodrage_enabled_{false};
    bool debug_mode_{false};
    bool use_fast_rng_{false};
    bool random_melee_hits_{false};
    bool use_mighty_rage_potion_{false};
    bool anger_management_enabled_{false};
    bool fuel_extra_rage_{false};
    double interval_;
    double damage_amount_;
    double glancing_factor_mh_{0.0};
    double glancing_factor_oh_{0.0};
    double armor_reduction_factor_{};
    Time_keeper time_keeper_{};
    std::default_random_engine eng_;
    std::uniform_real_distribution<double> dist100_;
    std::uniform_real_distribution<double> dist1_;

};

std::ostream &operator<<(std::ostream &os, Combat_simulator::Stat_weight const &stats);

#include "Combat_simulator.tcc"

#endif //WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
