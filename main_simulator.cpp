#include <iostream>
#include <ctime>

//#include "wow_library/include/Stats.hpp"
#include "wow_library/include/Combat_simulator.hpp"
#include "wow_library/include/Character.hpp"
#include "wow_library/include/Enchant.hpp"
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

Character create_character()
{
    Armory armory;
    Buffs buffs;

    Character character{Race::human};

    character.equip_armor(
            //Helmet
            armory.helmet.lionheart_helm,

            // Neck
            armory.neck.onyxia_tooth_pendant,

            // Shoulders
            armory.shoulder.truestrike_shoulders,

            // Back
            armory.back.cape_of_the_black_baron,

            // Chest
            armory.chest.savage_gladiator_chain,

            // Wrists
            armory.wrist.wristguards_of_stability,

            // Hands
            armory.hands.flameguard_gauntlets,

            // Belt
            armory.belt.onslaught_girdle,

            // Legs
            armory.legs.legguards_of_the_fallen_crusader,

            // boots
            armory.boots.chromatic_boots,

            // Rings
            armory.rings.don_julios_band,
            armory.rings.quick_strike_ring,

            // Trinket
            armory.trinket.blackhands_breadth,
            armory.trinket.diamond_flask,

            // Bow
            armory.ranged.strikers_mark
                         );

    character.equip_weapon(armory.swords.maladath,
                           armory.swords.brutality_blade);

    character.add_enchants(Enchant{Enchant::Socket::head, Enchant::Type::haste},
                           Enchant{Enchant::Socket::back, Enchant::Type::agility},
                           Enchant{Enchant::Socket::chest, Enchant::Type::major_stats},
                           Enchant{Enchant::Socket::wrists, Enchant::Type::strength9},
                           Enchant{Enchant::Socket::hands, Enchant::Type::haste},
                           Enchant{Enchant::Socket::legs, Enchant::Type::haste},
                           Enchant{Enchant::Socket::weapon_mh, Enchant::Type::crusader},
                           Enchant{Enchant::Socket::weapon_oh, Enchant::Type::crusader}
                          );

    character.add_buffs(
            buffs.rallying_cry,
            buffs.dire_maul,
            buffs.songflower,
            buffs.blessing_of_kings,
            buffs.blessing_of_might,
            buffs.gift_of_the_wild,
            buffs.trueshot_aura,
            buffs.elixir_mongoose,
            buffs.dense_stone_mh,
            buffs.dense_stone_oh,
//            buffs.elemental_stone_mh,
//            buffs.elemental_stone_oh,
            buffs.blessed_sunfruit,
            buffs.juju_power,
            buffs.juju_might,
            buffs.roids
                       );

    character.compute_all_stats(Character::Talent::fury, armory.get_set_bonuses());
    return character;
}

Character delta_gear(const Character &character)
{
    Armory armory;
    Buffs buffs;

    Character delta_character = character;

    delta_character.change_weapon(armory.swords.brutality_blade, Hand::main_hand);
    delta_character.change_weapon(armory.swords.maladath, Hand::off_hand);

//    delta_character.change_armor(armory.helmet.crown_of_destruction);
//    delta_character.change_armor(armory.trinket.drake_fang_talisman, false);

    delta_character.compute_all_stats(Character::Talent::fury, armory.get_set_bonuses());
    return delta_character;
}

int main()
{
    clock_t startTime = clock();
    std::vector<Character> characters;
    Armory armory;

    characters.emplace_back(create_character());
    characters.emplace_back(delta_gear(characters[0]));

    // Combat settings
    int n_batches = 20;
    double sim_time = 60;
    int opponent_level = 63;

    std::vector<Combat_simulator> simulators;
    simulators.emplace_back(Combat_simulator());
    simulators.emplace_back(Combat_simulator());

    simulators[0].use_fast_but_sloppy_rng(); // Use before set seed!
    simulators[0].set_seed(100); // Use for predictable random numbers
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

    simulators[1].use_fast_but_sloppy_rng(); // Use before set seed!
    simulators[1].set_seed(100); // Use for predictable random numbers
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
//    simulators[1].display_combat_debug();

    for (size_t i = 0; i < characters.size(); ++i)
    {
        if (!characters[i].check_if_armor_valid())
        {
            std::cout << "Char " << i + 1 << " has incorrect armor setup!\n";
            return -1;
        }
        if (!characters[i].check_if_weapons_valid())
        {
            std::cout << "Char " << i + 1 << " has incorrect weapon setup!\n";
            return -1;
        }
    }
    print_stat("Strength: ", characters[0].get_stats().strength, characters[1].get_stats().strength);
    print_stat("Agility : ", characters[0].get_stats().agility, characters[1].get_stats().agility);
    print_stat("Hit:    : ", characters[0].get_total_special_stats().hit, characters[1].get_total_special_stats().hit);
    print_stat("Crit    : ", characters[0].get_total_special_stats().critical_strike,
               characters[1].get_total_special_stats().critical_strike);
    print_stat("Atk Pwr : ", characters[0].get_total_special_stats().attack_power,
               characters[1].get_total_special_stats().attack_power);
    print_stat("Haste   : ", characters[0].get_haste(), characters[1].get_haste());
    print_stat("Extr Hit: ", characters[0].get_chance_for_extra_hit(), characters[1].get_chance_for_extra_hit());
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

