#include <iostream>
#include "sim_interface.hpp"

int main()
{
    Sim_interface sim_interface;

    Sim_input sim_input{60, 63, 10};
    auto sim_output = sim_interface.simulate(sim_input);

    for (double dmg_batch : sim_output.dmg_batches)
    {
        std::cout << "batch = " << dmg_batch << "\n";
    }

    std::cout << "DPS = " << sim_output.mean_dps;

    return 0;
}

