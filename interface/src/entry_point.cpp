#include "sim_interface.hpp"

#include <emscripten/bind.h>

//std::vector<double> covidTest(const CovidInput& input)
//{
//    return input.reproduction_scaling_y_coordinates;
//}

using namespace emscripten;

EMSCRIPTEN_BINDINGS(module)
{
//    function("covidTest", &covidTest);

    class_<Sim_interface>("Sim_interface")
        .constructor<>()
        .function("simulate", &Sim_interface::simulate);

    register_vector<double>("vector<double>");

    value_object<Sim_input>("Sim_input")
        .field("target_level", &Sim_input::target_level)
        .field("fight_time", &Sim_input::fight_time)
        .field("n_simulations", &Sim_input::n_simulations);

    value_object<Sim_output>("Sim_output")
        .field("dmg_batches", &Sim_output::dmg_batches)
        .field("mean_dps", &Sim_output::mean_dps)
        .field("std_dps", &Sim_output::std_dps);
};