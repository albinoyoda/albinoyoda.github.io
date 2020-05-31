#include <iostream>
#include "sim_interface.hpp"
#include <ctime>

int main()
{
    clock_t startTime = clock();
    Sim_interface sim_interface;

    std::vector<std::string> armor_vec;
    armor_vec.emplace_back("lionheart_helm");
    armor_vec.emplace_back("onyxia_tooth_pendant");
    armor_vec.emplace_back("drake_talon_pauldrons");
    armor_vec.emplace_back("cape_of_the_black_baron");
    armor_vec.emplace_back("savage_gladiator_chain");
    armor_vec.emplace_back("wristguards_of_stability");
    armor_vec.emplace_back("flameguard_gauntlets");
    armor_vec.emplace_back("onslaught_girdle");
    armor_vec.emplace_back("cloudkeeper_legplates");
    armor_vec.emplace_back("chromatic_boots");
    armor_vec.emplace_back("don_julios_band");
    armor_vec.emplace_back("master_dragonslayers_ring");
    armor_vec.emplace_back("hand_of_justice");
    armor_vec.emplace_back("diamond_flask");
    armor_vec.emplace_back("blastershot");

    std::vector<std::string> weapons_vec;
    weapons_vec.emplace_back("thunderfury_blessed_blade");
    weapons_vec.emplace_back("dal_rends_tribal_guardian");

    std::vector<std::string> buff_vec;
    buff_vec.emplace_back("rallying_cry");
    buff_vec.emplace_back("dire_maul");
    buff_vec.emplace_back("songflower");
    buff_vec.emplace_back("warchiefs_blessing");
    buff_vec.emplace_back("spirit_of_zandalar");
    buff_vec.emplace_back("sayges_fortune");
    buff_vec.emplace_back("windfury_totem");
    buff_vec.emplace_back("blessing_of_kings");
    buff_vec.emplace_back("strength_of_earth_totem");
    buff_vec.emplace_back("grace_of_air_totem");

    std::vector<std::string> ench_vec;
    ench_vec.emplace_back("h+8 strength");
    ench_vec.emplace_back("s+30 attack power");
    ench_vec.emplace_back("mcrusader");
    ench_vec.emplace_back("ocrusader");

    std::string race = "human";

    Sim_input sim_input{{race}, armor_vec, weapons_vec, buff_vec, ench_vec, {"hit", "crit"},
                        60, 63, 10000, 5, true, true,
                        true, true, true, false, true, false,
                        true, 60, 25, 1};

    auto sim_output = sim_interface.simulate(sim_input);

    for (const auto &dmg_batch : sim_output.stat_weights)
    {
        std::cout << dmg_batch << "\n";
    }

    for (double dmg_source : sim_output.dmg_sources)
    {
        std::cout << "source = " << dmg_source << "\n";
    }

    std::cout << "extra = " << "\n";
    for (const auto &message : sim_output.extra_stats)
    {
        std::cout << message << "\n";
    }

    std::cout << "DPS = " << sim_output.mean_dps << "\n";
    for (const auto &message : sim_output.messages)
    {
        std::cout << message << "\n";
    }

    std::cout << "Code executed in: " << double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds."
              << std::endl;

    return 0;
}

