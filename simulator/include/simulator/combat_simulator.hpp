#ifndef WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
#define WOW_SIMULATOR_COMBAT_SIMULATOR_HPP

#include "Character.hpp"
#include "ability_queue_manager.hpp"
#include "buff_manager.hpp"
#include "combat_simulator_config.hpp"
#include "cooldown_manager.hpp"
#include "damage_sources.hpp"
#include "flurry_manager.hpp"
#include "hit_tables.hpp"
#include "rage_manager.hpp"
#include "sim_input.hpp"
#include "sim_input_mult.hpp"
#include "slam_manager.hpp"
#include "spells.hpp"
#include "statistics/Distribution.hpp"
#include "time_keeper.hpp"
#include "weapon_sim.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <common/find_values.hpp>
#include <common/sim_time.hpp>
#include <common/string_helpers.hpp>
#include <iomanip>
#include <map>
#include <vector>

class Combat_simulator
{
public:
    Combat_simulator() = default;

    virtual ~Combat_simulator() = default;

    void set_config(const Combat_simulator_config& new_config);

    void swing_weapon(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                      Damage_sources& damage_sources, double attack_power_bonus = 0, bool is_extra_attack = false);

    void hit_effects(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                     Damage_sources& damage_sources, bool is_extra_attack = false);

    bool start_cast_slam(bool mh_swing, double rage, Sim_time& swing_time_left);

    void simulate(const Character& character, size_t n_simulations, double init_mean, double init_variance,
                  size_t init_simulations);

    void simulate(const Character& character, int init_iteration = 0, bool log_data = false, bool reset_dps = true);

    Hit_outcome generate_hit(const Weapon_sim& weapon, Damage_sources& damage_sources, double damage,
                             Hit_color hit_type, Socket weapon_hand, const Special_stats& special_stats,
                             bool boss_target = true, bool is_overpower = false, bool is_spell = false,
                             bool can_sweep = false);

    std::vector<std::pair<Sim_time, Use_effect>> get_use_effect_order(const Character& character);

    void cast_spell(const Spell_base& spell, Weapon_sim& weapon, Special_stats& special_stats);

    void slam(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage, Damage_sources& damage_sources);

    void mortal_strike(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                       Damage_sources& damage_sources);

    void bloodthirst(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                     Damage_sources& damage_sources);

    void whirlwind(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                   Damage_sources& damage_sources);

    void execute(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                 Damage_sources& damage_sources);

    void hamstring(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                   Damage_sources& damage_sources);

    void overpower(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                   Damage_sources& damage_sources);

    void add_damage_source_to_time_lapse(std::vector<Damage_instance>& damage_instances);

    [[nodiscard]] std::vector<std::string> get_aura_uptimes() const;

    [[nodiscard]] std::map<std::string, Sim_time> get_aura_uptimes_map() const { return buff_manager_.aura_uptime; };

    [[nodiscard]] std::map<std::string, int> get_proc_data() const { return proc_data_; };

    [[nodiscard]] std::vector<std::string> get_proc_statistics() const;

    void reset_time_lapse();

    [[nodiscard]] std::vector<std::vector<double>> get_damage_time_lapse() const;

    [[nodiscard]] std::string get_debug_topic() const;

    Damage_sources get_damage_distribution() { return damage_distribution_; }

    [[nodiscard]] constexpr Distribution get_dps_distribution() const { return dps_distribution_; }

    [[nodiscard]] constexpr double get_dps_mean() const { return dps_distribution_.mean_; }

    [[nodiscard]] constexpr double get_dps_variance() const { return dps_distribution_.variance_; }

    [[nodiscard]] constexpr int get_n_simulations() const { return config.n_batches; }

    [[nodiscard]] constexpr double get_rage_lost_stance() const { return rage_lost_stance_swap_; }

    [[nodiscard]] constexpr double get_rage_lost_capped() const { return rage_lost_capped_; }

    [[nodiscard]] constexpr double get_avg_rage_spent_executing() const { return avg_rage_spent_executing_; }

    std::vector<int>& get_hist_x() { return hist_x; }

    std::vector<int>& get_hist_y() { return hist_y; }

    [[nodiscard]] constexpr double get_flurry_uptime_mh() const { return flurry_uptime_mh_; }

    [[nodiscard]] constexpr double get_flurry_uptime_oh() const { return flurry_uptime_oh_; }

    [[nodiscard]] constexpr double get_hs_uptime() const { return heroic_strike_uptime_; }

    void init_histogram();

    void prune_histogram();

    void normalize_timelapse();

    void print_statement(const std::string& t) { debug_topic_ += t; }

    void print_statement(int t) { debug_topic_ += std::to_string(t); }

    void print_statement(double t) { debug_topic_ += std::to_string(t); }

    template <typename... Args>
    void simulator_cout(Args&&... args)
    {
        if (config.display_combat_debug)
        {
            //            s. Loop idx:" + std::to_string(                    time_keeper_.step_index) +=
            debug_topic_ += "Time: " + std::to_string(time_keeper_.time.seconds()) + "s. Event: ";
            __attribute__((unused)) int dummy[] = {0, ((void)print_statement(std::forward<Args>(args)), 0)...};
            debug_topic_ += "<br>";
        }
    }

    Combat_simulator_config config;

    Hit_tables hit_tables{};

private:
    Damage_sources damage_distribution_{};
    Time_keeper time_keeper_{};
    Cooldown_manager cooldown_manager_{};
    Buff_manager buff_manager_{};
    Flurry_manager flurry_manager_{};
    Rage_manager rage_manager_{};
    Ability_queue_manager ability_queue_manager{};
    Slam_manager slam_manager{};
    std::vector<int> hist_x{};
    std::vector<int> hist_y{};
    std::string debug_topic_{};

    Distribution dps_distribution_{};
    double armor_reduction_factor_{};
    double armor_reduction_factor_add{};
    int current_armor_red_stacks_{};
    int armor_penetration_{};
    int armor_reduction_from_spells_{};
    int armor_reduction_delayed_{};
    bool recompute_mitigation_{false};
    int number_of_extra_targets_{};

    double flurry_uptime_mh_{};
    double flurry_uptime_oh_{};
    double heroic_strike_uptime_{};

    int execute_rage_cost_{};
    int heroic_strike_rage_cost{};

    double cleave_bonus_damage_{};
    double rage_spent_on_execute_{};
    double avg_rage_spent_executing_{};
    double rage_lost_stance_swap_{};
    double rage_lost_capped_{};
    double p_unbridled_wrath_{};
    double flurry_haste_factor_{};
    double dual_wield_damage_factor_{};
    bool dpr_heroic_strike_queued_{false};
    bool dpr_cleave_queued_{false};

    double tactical_mastery_rage_{0};
    bool deep_wounds_{false};
    bool use_bloodthirst_{false};
    bool use_mortal_strike_{false};
    bool use_sweeping_strikes_{false};
    int sweeping_strikes_charges_ = 0;

    std::vector<std::vector<double>> damage_time_lapse{};
    std::map<std::string, int> proc_data_{};
    std::vector<Use_effect> use_effects_all_{};
    std::vector<Over_time_effect> over_time_effects_{};
    std::map<Damage_source, int> source_map{
        {Damage_source::white_mh, 0},         {Damage_source::white_oh, 1},         {Damage_source::bloodthirst, 2},
        {Damage_source::execute, 3},          {Damage_source::heroic_strike, 4},    {Damage_source::cleave, 5},
        {Damage_source::whirlwind, 6},        {Damage_source::hamstring, 7},        {Damage_source::deep_wounds, 8},
        {Damage_source::item_hit_effects, 9}, {Damage_source::overpower, 10},       {Damage_source::slam, 11},
        {Damage_source::mortal_strike, 12},   {Damage_source::sweeping_strikes, 13}};
};

#endif // WOW_SIMULATOR_SIMULATOR_HPP
