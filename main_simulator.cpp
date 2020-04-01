#include <iostream>
#include <ctime>

#include "wow_library/include/Combat_simulator.hpp"
#include "wow_library/include/Armory.hpp"

// TODO move weapon mechanics to the simulator instead of weapon class
// TODO Graphics?
// TODO HOJ proc when whirlwind etc.
// TODO Print the same data that is shown in warcraftlogs
// TODO 2h weapons

// TODO anger management
// TODO blood rage
// TODO deep wounds

// TODO move battle shout in the simulation
// TODO cooldowns
// TODO stances
// TODO overpower

// TODO setbonuses

// Notes
// Execute and bloodthirst refunds rage on dodge, not whirlwind
// Offhand hit chance is increased if heroic strike is activated

void print_stat(const std::string &stat_name, double amount_1, double amount_2)
{
    std::cout << stat_name << std::setw(6) << std::left << std::setprecision(6)
              << amount_1 << ", " << std::setw(5) << amount_2 << "\n";
}

void print_character_stat(const Character &character1, const Character &character2)
{
    print_stat("Strength: ", character1.total_attributes.strength, character2.total_attributes.strength);
    print_stat("Agility : ", character1.total_attributes.agility, character2.total_attributes.agility);
//    print_stat("Hit:    : ", charactersget_total_special_stats().hit, characters[1].get_total_special_stats().hit);
//    print_stat("Crit    : ", charactersget_total_special_stats().critical_strike,
//               characters[1].get_total_special_stats().critical_strike);
//    print_stat("Atk Pwr : ", characters[0].get_total_special_stats().attack_power,
//               characters[1].get_total_special_stats().attack_power);
//    print_stat("Haste   : ", characters[0].get_haste(), characters[1].get_haste());
//    print_stat("Extr Hit: ", characters[0].get_chance_for_extra_hit(), characters[1].get_chance_for_extra_hit());
    std::cout << "\n";
}

Character character_setup()
{
    Armory armory;
    Buffs buffs;

    Character character{Race::gnome, 60};

    // Armor
    character.equip_armor(armory.helmet.crown_of_destruction);
    character.equip_armor(armory.neck.onyxia_tooth_pendant);
    character.equip_armor(armory.shoulder.leutenant_shoulders);
    character.equip_armor(armory.back.cape_of_the_black_baron);
    character.equip_armor(armory.chest.cadaverous_armor);
    character.equip_armor(armory.wrist.vambraces_of_the_sadist);
    character.equip_armor(armory.hands.edgemasters_handguards);
    character.equip_armor(armory.belt.omokks_girth);
    character.equip_armor(armory.legs.knight_captain_plate_leggings);
    character.equip_armor(armory.boots.battlechasers);
    character.equip_armor(armory.rings.don_julios_band);
    character.equip_armor(armory.rings.magnis_will);
    character.equip_armor(armory.trinket.hand_of_justice);
    character.equip_armor(armory.trinket.diamond_flask);
    character.equip_armor(armory.ranged.satyrs_bow);

    // Weapons
    character.equip_weapon(armory.swords.thrash_blade, armory.maces.ebon_hand);

    // Enchants
    character.add_enchant(Socket::head, Enchant::Type::haste);
    character.add_enchant(Socket::back, Enchant::Type::agility);
    character.add_enchant(Socket::chest, Enchant::Type::major_stats);
    character.add_enchant(Socket::wrists, Enchant::Type::strength9);
    character.add_enchant(Socket::hands, Enchant::Type::haste);
    character.add_enchant(Socket::legs, Enchant::Type::haste);
//    character.add_enchant(Socket::boots, Enchant::Type::agility);
    character.add_enchant(Socket::main_hand, Enchant::Type::crusader);
    character.add_enchant(Socket::off_hand, Enchant::Type::crusader);

    character.add_buff(buffs.rallying_cry);
    character.add_buff(buffs.dire_maul);
    character.add_buff(buffs.songflower);
    character.add_buff(buffs.blessing_of_kings);
    character.add_buff(buffs.blessing_of_might);
    character.add_buff(buffs.gift_of_the_wild);
    character.add_buff(buffs.trueshot_aura);
    character.add_buff(buffs.elixir_mongoose);
    character.add_buff(buffs.blessed_sunfruit);
    character.add_buff(buffs.juju_power);
    character.add_buff(buffs.juju_might);
    character.add_buff(buffs.roids);

    character.add_weapon_buff(Socket::main_hand, buffs.dense_stone);
    character.add_weapon_buff(Socket::off_hand, buffs.dense_stone);

    armory.compute_total_stats(character);

    return character;
}

Character delta_gear(const Character &character)
{
    Armory armory;
    Buffs buffs;

    Character delta_character = character;

//    delta_character.change_weapon(armory.axes.crul_shorukh_edge_of_chaos, Hand::main_hand);

//    delta_character.change_armor(armory.chest.knight_captains_plate_hauberk);

//    delta_character.compute_all_stats(Character::Talent::fury, armory.get_set_bonuses());
    return delta_character;
}

int main()
{
    clock_t startTime = clock();
    Armory armory;

    Character character1 = character_setup();
    Character character2 = delta_gear(character1);

    print_character_stat(character1, character2);

    std::vector<Character> characters{character1, character2};

    // Combat settings
    int n_batches = 100000;
    double sim_time = 60;
    int opponent_level = 63;

    std::vector<Combat_simulator> simulators;
    simulators.emplace_back(Combat_simulator());
    simulators.emplace_back(Combat_simulator());

//    simulators[0].use_fast_but_sloppy_rng(); // Use before set seed!
//    simulators[0].set_seed(10000); // Use for predictable random numbers
//    simulators[0].enable_rng_melee(); // Uses random swing damage instead of average
    simulators[0].enable_spell_rotation();
//    simulators[0].use_heroic_spamm();
    simulators[0].use_mighty_rage_potion();
    simulators[0].enable_anger_management();
    simulators[0].enable_bloodrage();
//    simulators[0].fuel_extra_rage(3.0, 150);
    simulators[0].enable_talents();
    simulators[0].enable_item_chance_on_hit_effects();
    simulators[0].enable_crusader();
    simulators[0].enable_death_wish();
//    simulators[0].enable_recklessness();
//    simulators[0].display_combat_debug();

//    simulators[1].use_fast_but_sloppy_rng(); // Use before set seed!
//    simulators[1].set_seed(100); // Use for predictable random numbers
//    simulators[1].enable_rng_melee(); // Uses random swing damage instead of average
    simulators[1].enable_spell_rotation();
//    simulators[1].use_heroic_spamm();
    simulators[1].use_mighty_rage_potion();
    simulators[1].enable_anger_management();
    simulators[1].enable_bloodrage();
//    simulators[1].fuel_extra_rage(3.0, 150);
    simulators[1].enable_talents();
    simulators[1].enable_item_chance_on_hit_effects();
    simulators[1].enable_crusader();
    simulators[1].enable_death_wish();
//    simulators[1].enable_recklessness();
    simulators[1].display_combat_debug();

    print_stat("Strength: ", characters[0].total_attributes.strength, characters[1].total_attributes.strength);
    print_stat("Agility : ", characters[0].total_attributes.agility, characters[1].total_attributes.agility);
    print_stat("Hit:    : ", characters[0].total_special_stats.hit, characters[1].total_special_stats.hit);
    print_stat("Crit    : ", characters[0].total_special_stats.critical_strike,
               characters[1].total_special_stats.critical_strike);
    print_stat("Atk Pwr : ", characters[0].total_special_stats.attack_power,
               characters[1].total_special_stats.attack_power);
    print_stat("Haste   : ", characters[0].total_special_stats.haste, characters[1].total_special_stats.haste);
    print_stat("Extr Hit: ", characters[0].total_special_stats.chance_for_extra_hit,
               characters[1].total_special_stats.chance_for_extra_hit);
    std::cout << "\n";

    std::vector<std::vector<double>> dps_snapshots;
    std::vector<double> mean_dps;
    std::vector<double> std_dps;
    std::vector<double> sample_std_dps;
    for (size_t i = 0; i < characters.size(); ++i)
    {
        dps_snapshots.emplace_back(simulators[i].simulate(characters[i], sim_time, opponent_level, n_batches));
        mean_dps.emplace_back(Combat_simulator::average(dps_snapshots[i]));
        std_dps.emplace_back(Combat_simulator::standard_deviation(dps_snapshots[i], mean_dps[i]));
        sample_std_dps.emplace_back(Combat_simulator::sample_deviation(std_dps[i], n_batches));
        std::cout << std::setprecision(5);
        std::cout << "DPS from simulation: \n" << mean_dps[i] << " +- " << 1.96 * sample_std_dps[i]
                  << " (95% confidence interval)\n";
        std::cout << "DPS standard deviation in simulations: " << std_dps[i] << "\n";
        std::cout << "Crit % left to crit cap: " << 100 - simulators[i].get_hit_probabilities_white_mh().back()
                  << ". (Negative number means capped)\n\n";
        simulators[i].print_damage_distribution();

    }

    std::cout << "Simulations executed in: " << double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds."
              << std::endl;

//    for (size_t i = 0; i < characters.size(); ++i)
//    {
//        auto stat_weight_vector = simulators[i]
//                .compute_stat_weights(characters[i], armory, sim_time, opponent_level, n_batches, mean_dps[i],
//                                      sample_std_dps[i]);
//        std::cout << "Stat weights: \n";
//        for (const auto &stat_weight : stat_weight_vector)
//        {
//            std::cout << stat_weight;
//        }
//        std::cout << "\n";
//    }

    std::cout << "Code executed in: " << double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds."
              << std::endl;

    return 0;
}

