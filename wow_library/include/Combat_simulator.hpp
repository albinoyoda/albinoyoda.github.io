#ifndef WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
#define WOW_SIMULATOR_COMBAT_SIMULATOR_HPP

#include "Buff_manager.hpp"
#include "Character.hpp"
#include "Helper_functions.hpp"
#include "Statistics.hpp"
#include "damage_sources.hpp"
#include "sim_input.hpp"
#include "sim_input_mult.hpp"
#include "time_keeper.hpp"
#include "weapon_sim.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <map>
#include <vector>

struct Combat_simulator_config
{
    Combat_simulator_config() = default;

    explicit Combat_simulator_config(const Sim_input& input)
    {
        get_combat_simulator_config(input);
        n_batches = static_cast<int>(input.n_simulations);
        seed = 110000;
    };

    explicit Combat_simulator_config(const Sim_input_mult& input)
    {
        get_combat_simulator_config(input);
        seed = clock();
        performance_mode = true;
    };

    template <typename T>
    void get_combat_simulator_config(const T& input);

    // Combat settings
    int n_batches{};
    double sim_time{};
    int opponent_level{};

    bool exposed_armor{false};
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
    bool performance_mode{false};
    bool use_seed{false};
    int seed{};

    struct combat_t
    {
        bool use_bt_in_exec_phase{false};
        bool use_hs_in_exec_phase{false};
        double whirlwind_rage_thresh{};
        double overpower_rage_thresh{};
        double whirlwind_bt_cooldown_thresh{};
        double overpower_bt_cooldown_thresh{};
        double overpower_ww_cooldown_thresh{};
        double heroic_strike_rage_thresh{};
        double cleave_rage_thresh{};
        double heroic_strike_damage{};
        bool cleave_if_adds{false};
        bool use_hamstring{false};
        bool use_bloodthirst{false};
        bool use_whirlwind{false};
        bool use_overpower{false};
        bool use_heroic_strike{false};
        double hamstring_cd_thresh{};
        double hamstring_thresh_dd{};
        double initial_rage{};
        bool deep_wounds{false};
    } combat;

    struct talents_t
    {
        bool death_wish{false};
        bool anger_management{false};
        int improved_heroic_strike = 0;
        int flurry = 0;
        int unbridled_wrath = 0;
        int impale = 0;
        int overpower = 0;
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
    Combat_simulator() = default;

    virtual ~Combat_simulator() = default;

    void set_config(Combat_simulator_config& new_config)
    {
        config = new_config;
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
        bool is_ability_queued() { return heroic_strike_queued || cleave_queued; }

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

    void manage_flurry(Hit_result hit_result, Special_stats& special_stats, int& flurry_charges,
                       bool is_ability = false);

    void swing_weapon(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                      Damage_sources& damage_sources, int& flurry_charges, double attack_power_bonus = 0,
                      bool is_extra_attack = false);

    void hit_effects(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                     Damage_sources& damage_sources, int& flurry_charges, bool is_extra_attack = false);

    void overpower(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                   Damage_sources& damage_sources, int& flurry_charges);

    void bloodthirst(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                     Damage_sources& damage_sources, int& flurry_charges);

    void whirlwind(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                   Damage_sources& damage_sources, int& flurry_charges);

    void execute(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                 Damage_sources& damage_sources, int& flurry_charges, double execute_cost);

    void hamstring(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                   Damage_sources& damage_sources, int& flurry_charges);

    void simulate(const Character& character, size_t n_simulations, double init_mean, double init_variance,
                  size_t init_simulations);

    void simulate(const Character& character, size_t n_simulations);

    void simulate(const Character& character, int init_iteration = 0, bool compute_time_lape = false,
                  bool compute_histogram = false);

    static double get_uniform_random(double r_max) { return rand() * r_max / RAND_MAX; }

    static double get_uniform_random(double r_min, double r_max) { return r_min + rand() * (r_max - r_min) / RAND_MAX; }

    Combat_simulator::Hit_outcome generate_hit(const Weapon_sim& weapon, double damage, Hit_type hit_type,
                                               Socket weapon_hand, const Special_stats& special_stats,
                                               bool boss_target = true, bool is_overpower = false);

    Combat_simulator::Hit_outcome generate_hit_oh(double damage);

    Combat_simulator::Hit_outcome generate_hit_mh(double damage, Hit_type hit_type, bool is_overpower = false);

    void compute_hit_table(int level_difference, int weapon_skill, Special_stats special_stats, Socket weapon_hand);

    const std::vector<double>& get_hit_probabilities_white_mh() const;

    const std::vector<double>& get_hit_probabilities_white_oh() const;

    const std::vector<double>& get_hit_probabilities_white_2h() const;

    const std::vector<double>& get_hit_probabilities_yellow() const;

    double get_glancing_penalty_mh() const;

    double get_glancing_penalty_oh() const;

    void cout_damage_parse(Combat_simulator::Hit_type hit_type, Socket weapon_hand,
                           Combat_simulator::Hit_outcome hit_outcome);

    void add_damage_source_to_time_lapse(std::vector<Damage_instance>& damage_instances);

    Damage_sources get_damage_distribution() { return damage_distribution_; }

    std::vector<std::string> get_aura_uptimes() const;

    std::vector<std::string> get_proc_statistics() const;

    void reset_time_lapse();

    std::vector<std::vector<double>> get_damage_time_lapse() const;

    std::string get_debug_topic() const;

    constexpr double get_dps_mean() const { return dps_mean_; }

    constexpr double get_dps_variance() const { return dps_variance_; }

    constexpr int get_n_simulations() const { return config.n_batches; }

    constexpr int get_rage_lost_stance() const { return rage_lost_stance_swap_; }

    constexpr int get_rage_lost_exec() const { return rage_lost_execute_batch_; }

    constexpr int get_rage_lost_capped() const { return rage_lost_capped_; }

    std::vector<double>& get_hist_x() { return hist_x; }

    std::vector<int>& get_hist_y() { return hist_y; }

    void init_histogram();

    void prune_histogram();

    void normalize_timelapse();

    void print_statement(std::string t) { debug_topic_ += t; }

    void print_statement(int t) { debug_topic_ += std::to_string(t); }

    void print_statement(double t) { debug_topic_ += std::to_string(t); }

    template <typename... Args>
    void simulator_cout(Args&&... args)
    {
        if (config.display_combat_debug)
        {
            //            s. Loop idx:" + std::to_string(                    time_keeper_.step_index) +=
            debug_topic_ += "Time: " + std::to_string(time_keeper_.time) + "s. Event: ";
            __attribute__((unused)) int dummy[] = {0, ((void)print_statement(std::forward<Args>(args)), 0)...};
            debug_topic_ += "<br>";
        }
    }

    Combat_simulator_config config;

private:
    Use_effect deathwish = {
        "Death_wish", Use_effect::Effect_socket::unique, {}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, .20}, -10, 30, 180, true};

    Use_effect recklessness = {"Recklessness", Use_effect::Effect_socket::unique, {}, {100, 0, 0}, 0, 15, 900, true};

    Use_effect bloodrage = {"Bloodrage", Use_effect::Effect_socket::unique, {}, {}, 10, 10, 60, false,
                            {},          {{"Bloodrage", {}, 1, 0, 1, 10}}};

    Over_time_effect essence_of_the_red = {"Essence_of_the_red", {}, 20, 0, 1, 180};

    Over_time_effect unbridled_wrath = {"Unbridled_wrath", {}, 1, 0, 3, 600};

    std::vector<double> hit_table_white_mh_;
    std::vector<double> damage_multipliers_white_mh_;
    std::vector<double> hit_table_white_oh_;
    std::vector<double> damage_multipliers_white_oh_;
    std::vector<double> hit_table_yellow_;
    std::vector<double> hit_table_overpower_;
    std::vector<double> damage_multipliers_yellow_;
    std::vector<double> hit_table_two_hand_;
    Damage_sources damage_distribution_{};
    double dps_mean_;
    double dps_variance_;
    std::vector<double> hist_x{};
    std::vector<int> hist_y{};
    double armor_reduction_factor_{};
    double target_armor_{};
    double armor_reduction_factor_add{};
    double current_armor_red_stacks_{};
    Time_keeper time_keeper_{};
    Buff_manager buff_manager_{};
    Ability_queue_manager ability_queue_manager{};
    double flurry_uptime_mh_{};
    double flurry_uptime_oh_{};
    double heroic_strike_uptime_{};
    std::string debug_topic_{};
    int adds_in_melee_range{};
    double remove_adds_timer{};
    double heroic_strike_rage_cost{};
    double rage_lost_execute_batch_{};
    double rage_lost_stance_swap_{};
    double rage_lost_capped_{};
    std::vector<std::vector<double>> damage_time_lapse{};
    std::map<Damage_source, int> source_map{
        {Damage_source::white_mh, 0},         {Damage_source::white_oh, 1},      {Damage_source::bloodthirst, 2},
        {Damage_source::execute, 3},          {Damage_source::heroic_strike, 4}, {Damage_source::cleave, 5},
        {Damage_source::whirlwind, 6},        {Damage_source::hamstring, 7},     {Damage_source::deep_wounds, 8},
        {Damage_source::item_hit_effects, 9}, {Damage_source::overpower, 10}};
};

#include "Combat_simulator.tcc"

#endif // WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
