#include "sim_interface.hpp"

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(module)
{
    class_<Sim_interface>("Sim_interface")
        .constructor<>()
        .function("simulate", &Sim_interface::simulate)
        .function("simulate_mult", &Sim_interface::simulate_mult);

    register_vector<double>("vector<double>");
    register_vector<int>("vector<int>");
    register_vector<std::vector<double>>("vector<vector<double>>");
    register_vector<std::string>("StringList");

    value_object<Sim_input>("Sim_input")
        .field("race", &Sim_input::race)
        .field("armor", &Sim_input::armor)
        .field("weapons", &Sim_input::weapons)
        .field("buffs", &Sim_input::buffs)
        .field("enchants", &Sim_input::enchants)
        .field("stat_weights", &Sim_input::stat_weights)
        .field("options", &Sim_input::options)
        .field("compare_armor", &Sim_input::compare_armor)
        .field("compare_weapons", &Sim_input::compare_weapons)
        .field("target_level", &Sim_input::target_level)
        .field("fight_time", &Sim_input::fight_time)
        .field("n_simulations", &Sim_input::n_simulations)
        .field("n_simulations_stat_weights", &Sim_input::n_simulations_stat_weights)
        .field("sunder_armor", &Sim_input::sunder_armor)
        .field("heroic_strike_rage_thresh", &Sim_input::heroic_strike_rage_thresh)
        .field("cleave_rage_thresh", &Sim_input::cleave_rage_thresh)
        .field("whirlwind_rage_thresh", &Sim_input::whirlwind_rage_thresh)
        .field("whirlwind_bt_cooldown_thresh", &Sim_input::whirlwind_bt_cooldown_thresh)
        .field("hamstring_cd_thresh", &Sim_input::hamstring_cd_thresh)
        .field("hamstring_thresh_dd", &Sim_input::hamstring_thresh_dd)
        .field("initial_rage", &Sim_input::initial_rage);

    value_object<Sim_input_mult>("Sim_input_mult")
        .field("race", &Sim_input_mult::race)
        .field("armor", &Sim_input_mult::armor)
        .field("weapons", &Sim_input_mult::weapons)
        .field("buffs", &Sim_input_mult::buffs)
        .field("enchants", &Sim_input_mult::enchants)
        .field("options", &Sim_input_mult::options)
        .field("target_level", &Sim_input_mult::target_level)
        .field("fight_time", &Sim_input_mult::fight_time)
        .field("sunder_armor", &Sim_input_mult::sunder_armor)
        .field("heroic_strike_rage_thresh", &Sim_input_mult::heroic_strike_rage_thresh)
        .field("cleave_rage_thresh", &Sim_input_mult::cleave_rage_thresh)
        .field("whirlwind_rage_thresh", &Sim_input_mult::whirlwind_rage_thresh)
        .field("whirlwind_bt_cooldown_thresh", &Sim_input_mult::whirlwind_bt_cooldown_thresh)
        .field("hamstring_cd_thresh", &Sim_input_mult::hamstring_cd_thresh)
        .field("hamstring_thresh_dd", &Sim_input_mult::hamstring_thresh_dd)
        .field("initial_rage", &Sim_input_mult::initial_rage)
        .field("max_optimize_time", &Sim_input_mult::max_optimize_time);

    value_object<Sim_output>("Sim_output")
        .field("hist_x", &Sim_output::hist_x)
        .field("hist_y", &Sim_output::hist_y)
        .field("dmg_sources", &Sim_output::dmg_sources)
        .field("time_lapse_names", &Sim_output::time_lapse_names)
        .field("damage_time_lapse", &Sim_output::damage_time_lapse)
        .field("aura_uptimes", &Sim_output::aura_uptimes)
        .field("proc_counter", &Sim_output::proc_counter)
        .field("stat_weights", &Sim_output::stat_weights)
        .field("extra_stats", &Sim_output::extra_stats)
        .field("mean_dps", &Sim_output::mean_dps)
        .field("std_dps", &Sim_output::std_dps)
        .field("messages", &Sim_output::messages);

    value_object<Sim_output_mult>("Sim_output_mult")
        .field("messages", &Sim_output_mult::messages);

};