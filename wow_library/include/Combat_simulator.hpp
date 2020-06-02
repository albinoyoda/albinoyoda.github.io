#ifndef WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
#define WOW_SIMULATOR_COMBAT_SIMULATOR_HPP

#include <vector>
#include <cassert>
#include <map>
#include <cmath>
#include <iomanip>

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

    bool curse_of_recklessness_active{false};
    bool faerie_fire_feral_active{false};
    int n_sunder_armor_stacks = 0;

    // Simulator settings
    bool use_sim_time_ramp = false;
    bool enable_bloodrage{false};
    bool enable_recklessness{false};

    bool fuel_extra_rage{false};
    int extra_rage_interval{};
    int extra_rage_damage_amount{};

    bool display_combat_debug{false};
    bool use_seed{false};
    int seed{};

    struct combat_t
    {
        bool use_bt_in_exec_phase;
        bool use_hs_in_exec_phase;
        double whirlwind_rage_thresh;
        double whirlwind_bt_cooldown_thresh;
        double heroic_strike_rage_thresh;
        double cleave_rage_thresh;
        bool cleave_if_adds;
    } combat;

    struct talents_t
    {
        bool death_wish{false};
        bool anger_management{false};
        int improved_heroic_strike = 0;
        int flurry = 0;
        int unbridled_wrath = 0;
        int impale = 0;
        int improved_execute = 0;
        int dual_wield_specialization = 0;
    } talents;

    struct mode_t
    {
        bool sulfuron_harbinger{false};
        bool golemagg{false};
        bool vaelastrasz{false};
        bool chromaggus{false};
    } mode;
};

class Combat_simulator
{
public:
    explicit Combat_simulator(Combat_simulator_config config) : config(config)
    {
        if (config.use_seed)
        {
            srand(config.seed);
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

    enum class Hit_type
    {
        white,
        yellow
    };

    struct Ability_queue_manager
    {
        bool is_ability_queued()
        {
            return heroic_strike_queued || cleave_queued;
        }

        void queue_heroic_strike()
        {
            heroic_strike_queued = true;
            cleave_queued = false;
        }

        void queue_cleave()
        {
            heroic_strike_queued = false;
            cleave_queued = true;
        }

        void reset()
        {
            heroic_strike_queued = false;
            cleave_queued = false;
        }

        bool heroic_strike_queued{false};
        bool cleave_queued{false};
    };

    struct Hit_outcome
    {
        Hit_outcome()
        {
            damage = 0;
            hit_result = Hit_result::TBD;
        };

        Hit_outcome(double damage, Hit_result hit_result) : damage{damage}, hit_result{hit_result} {};

        double damage;
        Hit_result hit_result;
    };

    constexpr double lookup_outcome_mh(int case_id)
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
                return 2.0 + 0.1 * config.talents.impale;
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
                return 2.0 + 0.1 * config.talents.impale;
            case 4:
                return 1.0;
            default:
                assert(false);
                return 0.0;
        }
    }

    void
    manage_flurry(Hit_result hit_result, Special_stats &special_stats, int &flurry_charges, bool is_ability = false);

    void swing_weapon(Weapon_sim &weapon, Weapon_sim &main_hand_weapon, Special_stats &special_stats,
                      double &rage, bool &recklessness_active, Damage_sources &damage_sources, int &flurry_charges,
                      double attack_power_bonus = 0);

    void hit_effects(Weapon_sim &weapon, Weapon_sim &main_hand_weapon, Special_stats &special_stats,
                     double &rage, bool &recklessness_active, Damage_sources &damage_sources, int &flurry_charges);

    void bloodthirst(Weapon_sim &main_hand_weapon, Special_stats &special_stats,
                     double &rage, bool &recklessness_active, Damage_sources &damage_sources, int &flurry_charges);

    void whirlwind(Weapon_sim &main_hand_weapon, Special_stats &special_stats,
                   double &rage, bool &recklessness_active, Damage_sources &damage_sources, int &flurry_charges);

    void execute(Weapon_sim &main_hand_weapon, Special_stats &special_stats,
                 double &rage, bool &recklessness_active, Damage_sources &damage_sources, int &flurry_charges,
                 double execute_cost);


    void
    simulate(const Character &character, int n_batches, bool compute_time_lape = false, bool compute_histogram = false);

    void simulate(const Character &character, bool compute_time_lape = false, bool compute_histogram = false);

    static double get_uniform_random(double r_max)
    {
        return rand() * r_max / RAND_MAX;
    }

    Combat_simulator::Hit_outcome generate_hit(double damage, Hit_type hit_type, Socket weapon_hand,
                                               const Special_stats &special_stats, bool recklessness_active,
                                               bool boss_target = true);

    Combat_simulator::Hit_outcome generate_hit_oh(double damage, bool recklessness_active);

    Combat_simulator::Hit_outcome generate_hit_mh(double damage, Hit_type hit_type, bool recklessness_active);

    void compute_hit_table(int level_difference, int weapon_skill, Special_stats special_stats, Socket weapon_hand);

    const std::vector<double> &get_hit_probabilities_white_mh() const;

    const std::vector<double> &get_hit_probabilities_white_oh() const;

    const std::vector<double> &get_hit_probabilities_yellow() const;

    double get_glancing_penalty_mh() const;

    double get_glancing_penalty_oh() const;


    void cout_damage_parse(Combat_simulator::Hit_type hit_type, Socket weapon_hand,
                           Combat_simulator::Hit_outcome hit_outcome);

    void add_damage_source_to_time_lapse(std::vector<Damage_instance> &damage_instances);

    Damage_sources get_damage_distribution()
    {
        return damage_distribution_;
    }

    std::vector<std::string> get_aura_uptimes() const;

    std::vector<std::string> get_proc_statistics() const;

    void reset_time_lapse();

    std::vector<std::vector<double>> get_damage_time_lapse() const;

    std::string get_debug_topic() const;

    constexpr double get_dps_mean() const
    {
        return dps_mean_;
    }

    constexpr double get_dps_variance() const
    {
        return dps_variance_;
    }

    constexpr int get_n_simulations() const
    {
        return config.n_batches;
    }

    std::vector<double> &get_hist_x()
    {
        return hist_x;
    }

    std::vector<int> &get_hist_y()
    {
        return hist_y;
    }

    void init_histogram();

    void prune_histogram();

    void normalize_timelapse();

    void print_statement(std::string t)
    {
        debug_topic_ += t;
    }

    void print_statement(int t)
    {
        debug_topic_ += std::to_string(t);
    }

    void print_statement(double t)
    {

        debug_topic_ += std::to_string(t);
    }

    template<typename... Args>
    void simulator_cout(Args &&... args)
    {
        if (config.display_combat_debug)
        {
//            s. Loop idx:" + std::to_string(                    time_keeper_.step_index) +=
            debug_topic_ += "Time: " + std::to_string(time_keeper_.time) + "s. Event: ";
            __attribute__((unused)) int dummy[] = {0, ((void) print_statement(std::forward<Args>(args)), 0)...};
            debug_topic_ += "<br>";
        }
    }

    Combat_simulator_config config;

private:
    std::vector<double> hit_probabilities_white_mh_;
    std::vector<double> hit_probabilities_white_oh_;
    std::vector<double> hit_probabilities_yellow_;
    std::vector<double> hit_probabilities_recklessness_mh_;
    std::vector<double> hit_probabilities_recklessness_oh_;
    std::vector<double> hit_probabilities_recklessness_yellow_;
    std::vector<double> hit_probabilities_two_hand_;
    std::vector<double> hit_probabilities_recklessness_two_hand_;
    Damage_sources damage_distribution_{};
    double dps_mean_;
    double dps_variance_;
    std::vector<double> hist_x{};
    std::vector<int> hist_y{};
    double glancing_factor_mh_{};
    double glancing_factor_oh_{};
    double armor_reduction_factor_{};
    double target_armor_{};
    double armor_reduction_factor_add{};
    double current_armor_red_stacks_{};
    Time_keeper time_keeper_{};
    Buff_manager buff_manager_{};
    Ability_queue_manager ability_queue_manager{};
    double flurry_uptime_mh_{};
    double flurry_uptime_oh_{};
    std::string debug_topic_{};
    int adds_in_melee_range{};
    double remove_adds_timer{};
    double heroic_strike_rage_cost{};
    double cleave_rage_cost = 20;
    std::vector<std::vector<double>> damage_time_lapse{};
    std::map<Damage_source, int> source_map{{Damage_source::white_mh,         0},
                                            {Damage_source::white_oh,         1},
                                            {Damage_source::bloodthirst,      2},
                                            {Damage_source::execute,          3},
                                            {Damage_source::heroic_strike,    4},
                                            {Damage_source::cleave,           5},
                                            {Damage_source::whirlwind,        6},
                                            {Damage_source::item_hit_effects, 7}
    };
};

#include "Combat_simulator.tcc"

#endif //WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
