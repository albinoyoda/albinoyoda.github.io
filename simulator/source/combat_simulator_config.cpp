#include "simulator/combat_simulator_config.hpp"

Combat_simulator_config::Combat_simulator_config(const Sim_input& input)
{
    get_combat_simulator_config(input);

    n_batches = static_cast<int>(
        String_helpers::find_value(input.float_options_string, input.float_options_val, "n_simulations_dd"));
    if (String_helpers::find_string(input.options, "item_strengths") ||
        String_helpers::find_string(input.options, "wep_strengths") || !input.stat_weights.empty() ||
        String_helpers::find_string(input.options, "compute_dpr"))
    {
        if (n_batches < 100000)
        {
            std::cout << "Increased the number of simulations to 100000 to improve later calculations."
                      << std::endl;
            n_batches = 100000;
        }
    }
    seed = 110000;
};

Combat_simulator_config::Combat_simulator_config(const Sim_input_mult& input)
{
    get_combat_simulator_config(input);
    seed = clock();
    performance_mode = true;
};
