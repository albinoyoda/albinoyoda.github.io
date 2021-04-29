#include "Statistics.hpp"
#include "item_upgrades.hpp"

#include "gtest/gtest.h"

namespace
{
Item_upgrades_config get_item_upgrades_config(const size_t n_upgrades, const double dps_mean,
                                              const double dps_sample_std)
{
    return {50, 25, 500, 5000, n_upgrades, 1.2, dps_mean, dps_sample_std};
}

} // namespace

class Item_upgrades_fixture : public ::testing::Test
{
public:
    Item_upgrades_fixture() = default;

    Character character{Race::night_elf, 60};
    Armory armory{};

    void SetUp() override
    {
        Weapon wep = Weapon{"test_wep", {}, {}, 2.0, 100, 100, Weapon_socket::one_hand, Weapon_type::sword};
        character.equip_weapon(wep, wep);

        character.equip_armor(armory.find_armor(Socket::head, ""));
        character.equip_armor(armory.find_armor(Socket::neck, ""));
        character.equip_armor(armory.find_armor(Socket::shoulder, ""));
        character.equip_armor(armory.find_armor(Socket::back, ""));
        character.equip_armor(armory.find_armor(Socket::chest, ""));
        character.equip_armor(armory.find_armor(Socket::wrist, ""));
        character.equip_armor(armory.find_armor(Socket::hands, ""));
        character.equip_armor(armory.find_armor(Socket::belt, ""));
        character.equip_armor(armory.find_armor(Socket::legs, ""));
        character.equip_armor(armory.find_armor(Socket::boots, ""));
        character.equip_armor(armory.find_armor(Socket::ring, ""));
        character.equip_armor(armory.find_armor(Socket::ring, ""));
        character.equip_armor(armory.find_armor(Socket::trinket, ""));
        character.equip_armor(armory.find_armor(Socket::trinket, ""));
        character.equip_armor(armory.find_armor(Socket::ranged, ""));

        armory.compute_total_stats(character);
    }

    void TearDown() override
    {
        // Nothing to do since no memory was allocated
    }

    void add_item_to_character(const std::string& item_name, Socket socket, const bool first_misc_slot = true)
    {
        character.replace_armor(armory.find_armor(socket, item_name), first_misc_slot);
        armory.compute_total_stats(character);
    }

    [[nodiscard]] std::vector<Item_upgrade_result> get_item_upgrades(const Socket socket, int n_upgrades,
                                                                     const bool first_misc_slot = true,
                                                                     const bool use_seed = true) const
    {
        Combat_simulator_config simulator_config{};
        simulator_config.sim_time = 60.0;
        simulator_config.main_target_level = 63.0;
        simulator_config.performance_mode = true;
        simulator_config.n_batches = 10;
        if (use_seed)
        {
            simulator_config.seed = 110000;
        }
        Combat_simulator simulator{};
        simulator.set_config(simulator_config);
        int n_simulations = 10000;
        simulator.simulate(character, n_simulations, 0, 0, 0);

        double sample_std = simulator.get_std_of_the_mean();

        auto item_upgrades_config = get_item_upgrades_config(n_upgrades, simulator.get_dps_mean(), sample_std);

        Item_upgrades item_upgrades{simulator_config, item_upgrades_config};
        auto upgrades = item_upgrades.get_armor_upgrades(character, socket, first_misc_slot);
        return upgrades;
    }
};