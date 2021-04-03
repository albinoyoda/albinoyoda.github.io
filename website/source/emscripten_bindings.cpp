#include "emscripten/bind.h"
#include "sim_interface.hpp"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(module)
{
    class_<Sim_interface>("Sim_interface")
        .constructor<>()
        .function("simulate", &Sim_interface::simulate)
        .function("simulate_mult", &Sim_interface::simulate_mult);

    register_vector<double>("vectorDouble");
    register_vector<int>("vectorInt");
    register_vector<std::vector<double>>("vector<vector<double>>");
    register_vector<std::vector<std::string>>("StringListList");
    register_vector<std::string>("StringList");

    value_object<Sim_input>("Sim_input")
        .field("race", &Sim_input::race)
        .field("armor", &Sim_input::armor)
        .field("weapons", &Sim_input::weapons)
        .field("buffs", &Sim_input::buffs)
        .field("enchants", &Sim_input::enchants)
        .field("stat_weights", &Sim_input::stat_weights)
        .field("options", &Sim_input::options)
        .field("float_options_string", &Sim_input::float_options_string)
        .field("float_options_val", &Sim_input::float_options_val)
        .field("talent_string", &Sim_input::talent_string)
        .field("talent_val", &Sim_input::talent_val)
        .field("compare_armor", &Sim_input::compare_armor)
        .field("compare_weapons", &Sim_input::compare_weapons);

    value_object<Sim_input_mult>("Sim_input_mult")
        .field("race", &Sim_input_mult::race)
        .field("armor", &Sim_input_mult::armor)
        .field("weapons", &Sim_input_mult::weapons)
        .field("buffs", &Sim_input_mult::buffs)
        .field("enchants", &Sim_input_mult::enchants)
        .field("options", &Sim_input_mult::options)
        .field("float_options_string", &Sim_input_mult::float_options_string)
        .field("float_options_val", &Sim_input_mult::float_options_val)
        .field("talent_string", &Sim_input_mult::talent_string)
        .field("talent_val", &Sim_input_mult::talent_val);

    value_object<Sim_output>("Sim_output")
        .field("hist_x", &Sim_output::hist_x)
        .field("hist_y", &Sim_output::hist_y)
        .field("dmg_sources", &Sim_output::dmg_sources)
        .field("time_lapse_names", &Sim_output::time_lapse_names)
        .field("damage_time_lapse", &Sim_output::damage_time_lapse)
        .field("aura_uptimes", &Sim_output::aura_uptimes)
        .field("use_effect_order_string", &Sim_output::use_effect_order_string)
        .field("proc_counter", &Sim_output::proc_counter)
        .field("stat_weights", &Sim_output::stat_weights)
        .field("extra_stats", &Sim_output::extra_stats)
        .field("mean_dps", &Sim_output::mean_dps)
        .field("std_dps", &Sim_output::std_dps)
        .field("messages", &Sim_output::messages);

    value_object<Sim_output_mult>("Sim_output_mult")
        .field("messages", &Sim_output_mult::messages)
        .field("keepers", &Sim_output_mult::keepers);
};