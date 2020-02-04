#include <iostream>
#include <ctime>

#include "Stats.hpp"
#include "Item.hpp"
#include "Combat_simulator.hpp"
#include "Character.hpp"
#include "Enchant.hpp"

// TODO dynamic dt

// TODO buffs
// TODO crusader enchant / chance on hit

// TODO rage when missing?
// TODO heroic strike bug OH implementation

// TODO stances
// TODO move battle shout in the simulation / double check AP value
// TODO double check whirlwind damage
// TODO anger management
// TODO deep wounds
// TODO cooldowns
// TODO execute
// TODO overpower
// TODO setbonuses

struct Armory
{
    Armory()
    {
        set_extra_item_properties();
    }

    Armor lionheart_helm = Armor{Stats{18, 0}, Special_stats{2, 2, 0}, Armor::Socket::head};
    Armor onyxia_tooth_pendant = Armor{Stats{0, 13}, Special_stats{1, 1, 0}, Armor::Socket::neck};
    Armor truestrike_shoulders = Armor{Stats{0, 0}, Special_stats{0, 2, 24}, Armor::Socket::shoulder};
    Armor cape_of_the_black_baron = Armor{Stats{0, 15}, Special_stats{0, 0, 20}, Armor::Socket::back};
    Armor savage_gladiator_chain = Armor{Stats{13, 14}, Special_stats{2, 0, 0}, Armor::Socket::chest};
    Armor cadaverous_armor = Armor{Stats{8, 8}, Special_stats{0, 0, 60}, Armor::Socket::chest};
    Armor wristguards_of_stability = Armor{Stats{24, 0}, Special_stats{0, 0, 0}, Armor::Socket::wrists};
    Armor devilsaur_gauntlets = Armor{Stats{0, 0}, Special_stats{1, 1, 28}, Armor::Socket::hands};
    Armor edgemasters_handguards = Armor{Stats{0, 0}, Special_stats{0, 0, 0}, Armor::Socket::hands};
    Armor onslaught_girdle = Armor{Stats{31, 0}, Special_stats{1, 1, 0}, Armor::Socket::belt};
    Armor devilsaur_leggings = Armor{Stats{0, 0}, Special_stats{1, 1, 46}, Armor::Socket::legs};
    Armor windreaver_greaves = Armor{Stats{0, 20}, Special_stats{0, 1, 0}, Armor::Socket::boots};
    Armor bloodmail_boots = Armor{Stats{9, 9}, Special_stats{0, 1, 0}, Armor::Socket::boots};
    Armor don_julios_band = Armor{Stats{0, 0}, Special_stats{1, 1, 16}, Armor::Socket::ring1};
    Armor magnis_will = Armor{Stats{6, 0}, Special_stats{1, 0, 0}, Armor::Socket::ring2};
    Armor hand_of_justice = Armor{Stats{0, 0}, Special_stats{0, 0, 20}, Armor::Socket::trinket1};
    Armor blackhands_breadth = Armor{Stats{0, 0}, Special_stats{2, 0, 0}, Armor::Socket::trinket2};
    Armor satyrs_bow = Armor{Stats{0, 3}, Special_stats{0, 1, 0}, Armor::Socket::ranged};

    Weapon brutality_blade = Weapon{2.5, {90, 168}, Stats{9, 9}, Special_stats{1, 0, 0}, Weapon::Socket::main_hand};
    Weapon mirahs_song = Weapon{1.8, {57, 87}, Stats{9, 9}, Special_stats{0, 0, 0}, Weapon::Socket::off_hand};

    void set_extra_item_properties()
    {
        hand_of_justice.set_chance_for_extra_hit(2);
        edgemasters_handguards.set_extra_skill(7);
    }
};

int main()
{
    Character character{Race::human};
    Armory armory;

    character.equip_armor(armory.lionheart_helm,
                          armory.onyxia_tooth_pendant,
                          armory.truestrike_shoulders,
                          armory.cape_of_the_black_baron,
                          armory.savage_gladiator_chain,
//                          armory.cadaverous_armor,
                          armory.wristguards_of_stability,
                          armory.devilsaur_gauntlets,
//                          armory.edgemasters_handguards,
                          armory.onslaught_girdle,
                          armory.devilsaur_leggings,
                          armory.bloodmail_boots,
                          armory.don_julios_band,
                          armory.magnis_will,
                          armory.hand_of_justice,
                          armory.blackhands_breadth,
                          armory.satyrs_bow);

    character.equip_weapon(armory.brutality_blade,
                           armory.mirahs_song);

    if (!character.check_if_gear_valid())
    {
        std::cout << "Incorrect gear setup!\n";
        return -1;
    }

    character.add_enchants(Enchant{Enchant::Socket::head, Enchant::Type::haste},
                           Enchant{Enchant::Socket::back, Enchant::Type::agility},
                           Enchant{Enchant::Socket::chest, Enchant::Type::minor_stats},
                           Enchant{Enchant::Socket::wrists, Enchant::Type::strength7},
                           Enchant{Enchant::Socket::hands, Enchant::Type::strength},
                           Enchant{Enchant::Socket::legs, Enchant::Type::haste},
                           Enchant{Enchant::Socket::weapon_mh, Enchant::Type::crusader},
                           Enchant{Enchant::Socket::weapon_oh, Enchant::Type::crusader}
    );

    character.compute_all_stats(Character::Talent::fury);
    std::cout << character.get_stats() << "\n";
    std::cout << character.get_total_special_stats();
    std::cout << "haste: " << character.get_haste() << "\n";
    std::cout << "chance for extra hit: " << character.get_chance_for_extra_hit() << "%" << "\n\n";

    Combat_simulator combat_simulator;
    combat_simulator.enable_spell_rotation();
    combat_simulator.enable_talents();
    combat_simulator.enable_item_chance_on_hit_effects();
    combat_simulator.enable_crusader();
    //    srand(static_cast <unsigned> (time(nullptr)));

    int n_batches = 1000;
    auto dps_snapshots = combat_simulator.simulate(character, 120, .05, 63, n_batches);
    double mean_dps = Combat_simulator::average(dps_snapshots);
    double std_dps = Combat_simulator::standard_deviation(dps_snapshots, mean_dps);
    double sample_std_dps = Combat_simulator::sample_deviation(std_dps, n_batches);
    std::cout << "DPS from simulation: \n" << mean_dps << " +- " << 1.96 * sample_std_dps << "\n\n";

    auto stat_weight_vector = combat_simulator.compute_stat_weights(character, 100, 0.05, 63, n_batches);
    std::cout << "Stat weights: \n";
    for (const auto &stat_weight : stat_weight_vector)
    {
        std::cout << stat_weight;
    }


    return 0;
}
