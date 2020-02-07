#include <iostream>
#include <ctime>

#include "Stats.hpp"
#include "Item.hpp"
#include "Combat_simulator.hpp"
#include "Character.hpp"
#include "Enchant.hpp"
#include "Armory.hpp"

// TODO Graphics?
// TODO hit tables performance
// TODO recycle stat weight computations
// TODO damage sources std
// TODO HOJ proc when whirlwind etc.

// TODO 2h weapons

// TODO rage when missing?
// TODO heroic strike bug OH implementation

// TODO double check whirlwind damage

// TODO anger management
// TODO deep wounds

// TODO move battle shout in the simulation
// TODO cooldowns
// TODO stances
// TODO execute
// TODO overpower

// TODO setbonuses

int main()
{
    clock_t startTime = clock();

    Character character{Race::human};
    Armory armory;
    Buffs buffs;

    character.equip_armor(
//            armory.crown_of_destruction,
            armory.helm_of_endless_rage,
//            armory.lionheart_helm,
//            armory.lionheart_helm,
//            armory.expert_goldminers_head,

            armory.onyxia_tooth_pendant,
//            armory.prestors_talisman_of_connivery,

            armory.truestrike_shoulders,
//            armory.drake_talon_pauldrons,

//            armory.cloak_of_firemaw,
//            armory.cloak_of_draconic_might,
            armory.cape_of_the_black_baron,

            armory.savage_gladiator_chain,
//            armory.malfurions_blessed_bulwark,
//            armory.cadaverous_armor,

            armory.wristguards_of_stability,

//            armory.flameguard_gauntlets,
            armory.devilsaur_gauntlets,
//            armory.edgemasters_handguards,

            armory.onslaught_girdle,

            armory.devilsaur_leggings,
//            armory.legguards_of_the_fallen_crusader,

            armory.bloodmail_boots,
//            armory.chromatic_boots,
//            armory.boots_of_shadow_flame,

            armory.don_julios_band,
            armory.magnis_will,
//            armory.master_dragonslayers_ring,
//            armory.circle_of_applied_force,
//            armory.quick_strike_ring,

//            armory.hand_of_justice,
//            armory.drake_fang_talisman,
            armory.blackhands_breadth,
            armory.hand_of_justice,

            armory.satyrs_bow
//            armory.dragonbreath_hand_cannon
                         );

    character.equip_weapon(armory.brutality_blade,
                           armory.mirahs_song);

//    character.equip_weapon(armory.chromatically_tempered_sword,
//                           armory.maladath);

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

    character.add_enchants(Enchant{Enchant::Socket::head, Enchant::Type::haste},
                           Enchant{Enchant::Socket::back, Enchant::Type::agility},
                           Enchant{Enchant::Socket::chest, Enchant::Type::minor_stats},
                           Enchant{Enchant::Socket::wrists, Enchant::Type::strength7},
                           Enchant{Enchant::Socket::hands, Enchant::Type::haste},
                           Enchant{Enchant::Socket::legs, Enchant::Type::haste},
                           Enchant{Enchant::Socket::weapon_mh, Enchant::Type::crusader},
                           Enchant{Enchant::Socket::weapon_oh, Enchant::Type::crusader}
                          );

    character.add_buffs(
//            buffs.rallying_cry,
            buffs.dire_maul,
//            buffs.songflower,
            buffs.blessing_of_kings,
            buffs.blessing_of_might,
            buffs.mark_of_the_wild,
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

    character.compute_all_stats(Character::Talent::fury);
    std::cout << character.get_stats() << "\n";
    std::cout << character.get_total_special_stats();
    std::cout << "haste: " << character.get_haste() << "\n";
    std::cout << "chance for extra hit: " << character.get_chance_for_extra_hit() << "%" << "\n";

    Combat_simulator combat_simulator;

    // Combat settings
    combat_simulator.enable_spell_rotation();
    combat_simulator.enable_talents();
    combat_simulator.enable_item_chance_on_hit_effects();
    combat_simulator.enable_crusader();
//    combat_simulator.enable_death_wish();
//    combat_simulator.enable_recklessness();

//    srand(static_cast <unsigned> (time(nullptr)));

    int n_batches = 50000;
    double sim_time = 60;
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

