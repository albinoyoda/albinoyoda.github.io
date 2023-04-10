#ifndef WOW_SIMULATOR_COMBAT_SIMULATOR_CONFIG_HPP
#define WOW_SIMULATOR_COMBAT_SIMULATOR_CONFIG_HPP

#include "Character.hpp"
#include "buff_manager.hpp"
#include "common/find_values.hpp"
#include "common/string_helpers.hpp"
#include "damage_sources.hpp"
#include "sim_input.hpp"
#include "sim_input_mult.hpp"
#include "statistics/Distribution.hpp"
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

    explicit Combat_simulator_config(const Sim_input& input);

    explicit Combat_simulator_config(const Sim_input_mult& input);

    template <typename T>
    void get_combat_simulator_config(const T& input);

    // Combat settings
    int n_batches{};
    Sim_time sim_time{};

    int main_target_level{};
    int main_target_initial_armor_{};
    int extra_target_level{};
    int number_of_extra_targets{};
    double extra_target_percent_uptime{};
    int extra_target_initial_armor_{};
    int n_sunder_armor_stacks = 0;
    bool exposed_armor{false};
    bool curse_of_recklessness_active{false};
    bool faerie_fire_feral_active{false};

    bool take_periodic_damage_{};
    bool can_trigger_enrage_{};
    int periodic_damage_amount_{};
    Sim_time periodic_damage_interval_{};
    bool essence_of_the_red_{};

    double execute_phase_percentage_{};

    bool multi_target_mode_{};
    bool first_global_sunder_{};

    bool ability_queue_{};
    double ability_queue_rage_thresh_{};
    double berserking_haste_{};

    // Simulator settings
    bool enable_bloodrage{false};
    bool enable_recklessness{false};
    bool enable_blood_fury{false};
    bool enable_berserking{false};

    bool display_combat_debug{false};
    bool performance_mode{false};
    int seed{};

    struct combat_t
    {
        bool use_bt_in_exec_phase{false};
        bool use_ms_in_exec_phase{false};
        bool use_hs_in_exec_phase{false};
        double whirlwind_rage_thresh{};
        double overpower_rage_thresh{};
        Sim_time whirlwind_bt_cooldown_thresh{};
        Sim_time overpower_bt_cooldown_thresh{};
        Sim_time overpower_ww_cooldown_thresh{};
        double heroic_strike_rage_thresh{};
        double cleave_rage_thresh{};
        double heroic_strike_damage{};
        bool cleave_if_adds{false};
        bool use_hamstring{false};
        bool use_slam{false};
        bool use_bloodthirst{false};
        bool use_mortal_strike{false};
        bool use_sweeping_strikes{false};
        bool use_whirlwind{false};
        bool use_overpower{false};
        bool use_heroic_strike{false};
        Sim_time hamstring_cd_thresh{};
        bool dont_use_hm_when_ss{false};
        Sim_time slam_cd_thresh{};
        double hamstring_thresh_dd{};
        double initial_rage{};
        bool deep_wounds{false};
        bool first_hit_heroic_strike{false};
        double slam_spam_rage{false};
        Sim_time slam_spam_max_time{};
        double slam_rage_dd{false};
        bool use_death_wish{false};
    } combat;

    struct dpr_t
    {
        bool compute_dpr_sl_{false};
        bool compute_dpr_ms_{false};
        bool compute_dpr_bt_{false};
        bool compute_dpr_op_{false};
        bool compute_dpr_ww_{false};
        bool compute_dpr_ex_{false};
        bool compute_dpr_ha_{false};
        bool compute_dpr_hs_{false};
        bool compute_dpr_cl_{false};
    } dpr_settings;

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
        int improved_cleave = 0;
        int improved_slam = 0;
        int tactical_mastery = 0;
        int deep_wounds = 0;
        int bloodthirst = 0;
        int mortal_strike = 0;
        int sweeping_strikes = 0;
    } talents;
};

#include "combat_simulator_config.tcc"

#endif // WOW_SIMULATOR_COMBAT_SIMULATOR_CONFIG_HPP
