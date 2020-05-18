#include "sim_interface.hpp"

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(module)
{

    class_<Sim_interface>("Sim_interface")
        .constructor<>()
        .function("simulate", &Sim_interface::simulate);

    register_vector<double>("vector<double>");
    register_vector<std::string>("StringList");

    value_object<Sim_input>("Sim_input")
        .field("armor", &Sim_input::armor)
        .field("weapons", &Sim_input::weapons)
        .field("buffs", &Sim_input::buffs)
        .field("target_level", &Sim_input::target_level)
        .field("fight_time", &Sim_input::fight_time)
        .field("n_simulations", &Sim_input::n_simulations);

    value_object<Sim_output>("Sim_output")
        .field("dmg_batches", &Sim_output::dmg_batches)
        .field("mean_dps", &Sim_output::mean_dps)
        .field("std_dps", &Sim_output::std_dps)
        .field("messages", &Sim_output::messages);
};