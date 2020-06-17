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
    armor_vec.emplace_back("might_of_cenarius");
    armor_vec.emplace_back("master_dragonslayers_ring");
    armor_vec.emplace_back("badge_of_the_swarmguard");
    armor_vec.emplace_back("diamond_flask");
    armor_vec.emplace_back("blastershot");

    std::vector<std::string> weapons_vec;
    weapons_vec.emplace_back("thunderfury_blessed_blade");
    weapons_vec.emplace_back("dal_rends_tribal_guardian");

    std::vector<std::string> compare_armor_vec;
    compare_armor_vec.emplace_back("lionheart_helm");
    compare_armor_vec.emplace_back("onyxia_tooth_pendant");
    compare_armor_vec.emplace_back("truestrike_shoulders");
    compare_armor_vec.emplace_back("cape_of_the_black_baron");
    compare_armor_vec.emplace_back("savage_gladiator_chain");
    compare_armor_vec.emplace_back("wristguards_of_stability");
    compare_armor_vec.emplace_back("flameguard_gauntlets");
    compare_armor_vec.emplace_back("onslaught_girdle");
    compare_armor_vec.emplace_back("cloudkeeper_legplates");
    compare_armor_vec.emplace_back("chromatic_boots");
    compare_armor_vec.emplace_back("might_of_cenarius");
    compare_armor_vec.emplace_back("master_dragonslayers_ring");
    compare_armor_vec.emplace_back("badge_of_the_swarmguard");
    compare_armor_vec.emplace_back("diamond_flask");
    compare_armor_vec.emplace_back("blastershot");

    std::vector<std::string> compare_weapons_vec;
    compare_weapons_vec.emplace_back("thrash_blade");
    compare_weapons_vec.emplace_back("assassination_blade");

    std::vector<std::string> mult_armor_vec;
    mult_armor_vec.emplace_back("lionheart_helm");
    mult_armor_vec.emplace_back("onyxia_tooth_pendant");
    mult_armor_vec.emplace_back("truestrike_shoulders");
    mult_armor_vec.emplace_back("cape_of_the_black_baron");
    mult_armor_vec.emplace_back("savage_gladiator_chain");
    mult_armor_vec.emplace_back("wristguards_of_stability");
    mult_armor_vec.emplace_back("flameguard_gauntlets");
    mult_armor_vec.emplace_back("onslaught_girdle");
    mult_armor_vec.emplace_back("cloudkeeper_legplates");
    mult_armor_vec.emplace_back("chromatic_boots");
    mult_armor_vec.emplace_back("might_of_cenarius");
    mult_armor_vec.emplace_back("master_dragonslayers_ring");
    mult_armor_vec.emplace_back("badge_of_the_swarmguard");
    mult_armor_vec.emplace_back("diamond_flask");
    mult_armor_vec.emplace_back("blastershot");
    mult_armor_vec.emplace_back("hand_of_justice");
    mult_armor_vec.emplace_back("darkmoon_card_maelstrom");
//    mult_armor_vec.emplace_back("rune_of_the_guard_captain");
//    mult_armor_vec.emplace_back("jom_gabbar");
//    mult_armor_vec.emplace_back("earthstrike");
//    mult_armor_vec.emplace_back("magnis_will");
//    mult_armor_vec.emplace_back("quick_strike_ring");
//    mult_armor_vec.emplace_back("master_dragonslayers_ring");
//    mult_armor_vec.emplace_back("circle_of_applied_force");
//    mult_armor_vec.emplace_back("blackstone_ring");
//    mult_armor_vec.emplace_back("might_of_cenarius");
//    mult_armor_vec.emplace_back("master_dragonslayers_ring");
//    mult_armor_vec.emplace_back("windreaver_greaves");
//    mult_armor_vec.emplace_back("black_dragonscale_boots");
//    mult_armor_vec.emplace_back("conquerors_legguards");

    std::vector<std::string> mult_weapons_vec;
    mult_weapons_vec.emplace_back("dal_rends_tribal_guardian");
    mult_weapons_vec.emplace_back("thunderfury_blessed_blade");
    mult_weapons_vec.emplace_back("thrash_blade");
    mult_weapons_vec.emplace_back("assassination_blade");
    mult_weapons_vec.emplace_back("mirahs_song");
    mult_weapons_vec.emplace_back("viskag");
    mult_weapons_vec.emplace_back("quel_serrar");
    mult_weapons_vec.emplace_back("ancient_quiraji_ripper");

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

    std::vector<std::string> sim_options = {"faerie_fire", "curse_of_recklessness", "death_wish",
                                            "mighty_rage_potion", "sulfuron_harbinger",
                                            "debug_on", "use_bt_in_exec_phase", "use_hs_in_exec_phase",
                                            "cleave_if_adds", "use_hamstring"};

    Sim_input sim_input{{race}, armor_vec, weapons_vec, buff_vec, ench_vec, {"hit", "crit"}, sim_options,
                        compare_armor_vec, compare_weapons_vec, mult_armor_vec, mult_weapons_vec, 60, 60,
                        10000, 1000, 5, 60, 60,
                        25, 1, 2.0, 80, 45};

    auto sim_output = sim_interface.simulate_mult(sim_input);
//    sim_output = sim_interface.simulate(sim_input);

//    for (const auto &dmg_batch : sim_output.stat_weights)
//    {
//        std::cout << dmg_batch << "\n";
//    }
//
//    for (double dmg_source : sim_output.dmg_sources)
//    {
//        std::cout << "source = " << dmg_source << "\n";
//    }
//
//    std::cout << "extra = " << "\n";
//    for (const auto &message : sim_output.extra_stats)
//    {
//        std::cout << message << "\n";
//    }
//
//    for (const auto &dps : sim_output.mean_dps)
//    {
//        std::cout << "DPS = " << dps << "\n";
//    }

    for (const auto &message : sim_output.messages)
    {
        std::cout << message << "\n";
    }

    std::cout << "Code executed in: " << double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds."
              << std::endl;

    return 0;
}

