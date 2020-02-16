#include <iostream>
#include <ctime>

#include "Stats.hpp"
#include "Combat_simulator.hpp"
#include "Character.hpp"
#include "Enchant.hpp"
#include "Armory.hpp"

// TODO does missing refund rage?
// TODO move weapon mechanics to the simulator instead of weapon class
// TODO Graphics?
// TODO HOJ proc when whirlwind etc.

// TODO 2h weapons

// TODO heroic strike bug OH implementation

// TODO anger management
// TODO deep wounds

// TODO move battle shout in the simulation
// TODO cooldowns
// TODO stances
// TODO overpower

// TODO setbonuses

// Notes
// Execute and bloodthirst refunds rage on dodge, not whirlwind tho
// Offhand hit chance is increased if heroic strike is activated

int main()
{
    clock_t startTime = clock();

    Character character{Race::human};
    Armory armory;
    Buffs buffs;

    character.equip_armor(
            //Helmet
            armory.lionheart_helm,

            // Neck
            armory.onyxia_tooth_pendant,

            // Shoulders
            armory.truestrike_shoulders,

            // Back
            armory.cape_of_the_black_baron,

            // Chest
            armory.savage_gladiator_chain,

            // Wrists
            armory.wristguards_of_stability,

            // Hands
            armory.devilsaur_gauntlets,

            // Waist
            armory.onslaught_girdle,

            // Legs
            armory.devilsaur_leggings,

            // boots
            armory.bloodmail_boots,

            // Rings
            armory.don_julios_band,
            armory.magnis_will,

            // Trinket
            armory.blackhands_breadth,
            armory.hand_of_justice,

            // Bow
            armory.satyrs_bow
                         );

    character.equip_weapon(armory.brutality_blade,
                           armory.mirahs_song);

    character.add_enchants(Enchant{Enchant::Socket::head, Enchant::Type::haste},
                           Enchant{Enchant::Socket::back, Enchant::Type::agility},
                           Enchant{Enchant::Socket::chest, Enchant::Type::minor_stats},
                           Enchant{Enchant::Socket::wrists, Enchant::Type::strength7},
                           Enchant{Enchant::Socket::hands, Enchant::Type::strength},
                           Enchant{Enchant::Socket::legs, Enchant::Type::haste},
                           Enchant{Enchant::Socket::weapon_mh, Enchant::Type::crusader},
                           Enchant{Enchant::Socket::weapon_oh, Enchant::Type::crusader}
                          );

    character.add_buffs(
            buffs.rallying_cry,
            buffs.dire_maul,
//            buffs.songflower,
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
//            buffs.juju_might,
            buffs.roids
                       );

    if (!character.check_if_armor_valid())
    {
        std::cout << "Incorrect armor setup!\n";
        return -1;
    }
    if (!character.check_if_weapons_valid())
    {
        std::cout << "Incorrect weapon setup!\n";
        return -1;
    }

    character.compute_all_stats(Character::Talent::fury);
    std::cout << character.get_stats() << "\n";
    std::cout << character.get_total_special_stats();
    std::cout << "haste: " << character.get_haste() << "\n";
    std::cout << "chance for extra hit: " << character.get_chance_for_extra_hit() << "%" << "\n";

    Combat_simulator combat_simulator;
    combat_simulator.use_fast_but_sloppy_rng(); // Use before set seed!
    combat_simulator.set_seed(10); // Use for predictable random numbers
    combat_simulator.enable_rng_melee(); // Uses random swing damage instead of average

    // Combat settings
    combat_simulator.enable_spell_rotation();
    combat_simulator.enable_talents();
    combat_simulator.enable_item_chance_on_hit_effects();
    combat_simulator.enable_crusader();
    combat_simulator.enable_death_wish();
//    combat_simulator.enable_recklessness();
//    combat_simulator.display_combat_debug();


    int n_batches = 50000;
    double sim_time = 100;
    auto dps_snapshots = combat_simulator.simulate(character, sim_time, 63, n_batches);

    auto hit_table = combat_simulator.get_hit_probabilities_white_mh();
    double mean_dps = Combat_simulator::average(dps_snapshots);
    double std_dps = Combat_simulator::standard_deviation(dps_snapshots, mean_dps);
    double sample_std_dps = Combat_simulator::sample_deviation(std_dps, n_batches);
    std::cout << "Crit % left to crit cap: " << 100 - hit_table.back() << ". (Negative number means capped)\n\n";
    std::cout << "DPS from simulation: \n" << mean_dps << " +- " << 1.96 * sample_std_dps
              << " (95% confidence interval)\n";
    std::cout << "DPS standard deviation in simulations: " << std_dps << "\n\n";

    combat_simulator.print_damage_distribution();

    std::cout << "Code executed in: " << double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds."
              << std::endl;

    auto stat_weight_vector = combat_simulator
            .compute_stat_weights(character, sim_time, 63, n_batches, mean_dps, sample_std_dps);
    std::cout << "Stat weights: \n";
    for (const auto &stat_weight : stat_weight_vector)
    {
        std::cout << stat_weight;
    }

    std::cout << "Code executed in: " << double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds."
              << std::endl;

    return 0;
}

