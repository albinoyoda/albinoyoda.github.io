#include <iostream>
#include <ctime>

#include "wow_library/include/Combat_simulator.hpp"
#include "wow_library/include/Character.hpp"
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
    print_stat("Agility : ", character2.total_attributes.agility, character2.total_attributes.agility);
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

    Character character{Race::human, 60};

    // Armor
    character.equip_armor(armory.helmet.crown_of_destruction);
    character.equip_armor(armory.neck.onyxia_tooth_pendant);
    character.equip_armor(armory.shoulder.truestrike_shoulders);
    character.equip_armor(armory.back.cape_of_the_black_baron);
    character.equip_armor(armory.chest.cadaverous_armor);
    character.equip_armor(armory.wrist.vambraces_of_the_sadist);
    character.equip_armor(armory.hands.devilsaur_gauntlets);
    character.equip_armor(armory.belt.omokks_girth);
    character.equip_armor(armory.legs.knight_captain_plate_leggings);
    character.equip_armor(armory.boots.battlechasers);
    character.equip_armor(armory.rings.don_julios_band);
    character.equip_armor(armory.rings.magnis_will);
    character.equip_armor(armory.trinket.blackhands_breadth);
    character.equip_armor(armory.trinket.diamond_flask);
    character.equip_armor(armory.ranged.satyrs_bow);

    // Weapons
    character.equip_weapon(armory.swords.maladath, armory.swords.brutality_blade);

    // Enchants
    character.add_enchant(Socket::head, Enchant::Type::haste);
    character.add_enchant(Socket::back, Enchant::Type::agility);
    character.add_enchant(Socket::chest, Enchant::Type::major_stats);
    character.add_enchant(Socket::wrists, Enchant::Type::strength9);
    character.add_enchant(Socket::hands, Enchant::Type::haste);
    character.add_enchant(Socket::legs, Enchant::Type::haste);
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
    character.add_weapon_buff(Socket::main_hand, buffs.dense_stone);

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

    Character character1 = character_setup();
    Character character2 = delta_gear(character1);

    print_character_stat(character1, character2);

    // Combat settings
    Combat_simulator_config config{};
    config.sim_time = 60;
    config.target_level = 63;
    config.n_damage_batches = 1;

    config.talent_improved_heroic_strike = 2;

    Combat_simulator simulator(config);
    simulator.simulate(character1);
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

//    }

    std::cout << "Simulations executed in: " << double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds."
              << std::endl;

    for (size_t i = 0; i < characters.size(); ++i)
    {
        auto stat_weight_vector = simulators[i]
                .compute_stat_weights(characters[i], armory, sim_time, opponent_level, n_batches, mean_dps[i],
                                      sample_std_dps[i]);
        std::cout << "Stat weights: \n";
        for (const auto &stat_weight : stat_weight_vector)
        {
            std::cout << stat_weight;
        }
        std::cout << "\n";
    }

    std::cout << "Code executed in: " << double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds."
              << std::endl;

    return 0;
}

