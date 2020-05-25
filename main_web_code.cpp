#include <iostream>
#include "sim_interface.hpp"

int main()
{
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
    armor_vec.emplace_back("legguards_of_the_fallen_crusader");
    armor_vec.emplace_back("chromatic_boots");
    armor_vec.emplace_back("don_julios_band");
    armor_vec.emplace_back("master_dragonslayers_ring");
    armor_vec.emplace_back("hand_of_justice");
    armor_vec.emplace_back("diamond_flask");
    armor_vec.emplace_back("blastershot");

    std::vector<std::string> weapons_vec;
    weapons_vec.emplace_back("maladath");
    weapons_vec.emplace_back("deathbringer");

    std::vector<std::string> buff_vec;
    buff_vec.emplace_back("rallying_cry");
    buff_vec.emplace_back("dire_maul");
    buff_vec.emplace_back("songflower");
    buff_vec.emplace_back("warchiefs_blessing");
    buff_vec.emplace_back("spirit_of_zandalar");
    buff_vec.emplace_back("sayges_fortune");

    std::vector<std::string> ench_vec;
    ench_vec.emplace_back("h+8 strength");
    ench_vec.emplace_back("s+30 attack power");

    std::string race = "human";

    Sim_input sim_input{{race}, armor_vec, weapons_vec, buff_vec, ench_vec, {"crit", "hit", "haste"},
                        60, 63, 10000, 5, true, true,
                        true, true, true, false};

    auto sim_output = sim_interface.simulate(sim_input);

    for (auto dmg_batch : sim_output.stat_weights)
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

    return 0;
}

