
#include "item_upgrades.hpp"

#include "gtest/gtest.h"
#include <Statistics.hpp>

namespace
{
Item_upgrades_config get_item_upgrades_config(const double dps_mean, const double dps_sample_std)
{
    return {100, 25, 500, 5000, 5, 1.2, dps_mean, dps_sample_std};
}

Item_upgrades get_default_item_upgrades()
{
    Character character{Race::gnome, 60};
    Combat_simulator_config simulator_config{};

    Combat_simulator simulator{};
    simulator.set_config(simulator_config);
    int n_simulations = 10000;
    simulator.simulate(character, n_simulations, 0, 0, 0);

    double sample_std = Statistics::sample_deviation(std::sqrt(simulator.get_dps_variance()), n_simulations);

    return Item_upgrades{simulator_config, get_item_upgrades_config(simulator.get_dps_mean(), sample_std)};
}

} // namespace

class Item_upgrades_fixture : public ::testing::Test
{
public:
    Item_upgrades_fixture() = default;

    void SetUp() override {}

    void TearDown() override
    {
        // Nothing to do since no memory was allocated
    }

    std::vector<Item_upgrade_result> get_item_upgrades(const Character& character, const bool use_seed = true)
    {
        Combat_simulator_config simulator_config{};
        Combat_simulator simulator{};
        simulator.set_config(simulator_config);
        int n_simulations = 10000;
        simulator.simulate(character, n_simulations, 0, 0, 0);

        double sample_std = Statistics::sample_deviation(std::sqrt(simulator.get_dps_variance()), n_simulations);

        Item_upgrades item_upgrades{simulator_config, get_item_upgrades_config(simulator.get_dps_mean(), sample_std)};
        auto upgrades = item_upgrades.get_armor_upgrades(character, Socket::head, true);
        return upgrades;
    }
};