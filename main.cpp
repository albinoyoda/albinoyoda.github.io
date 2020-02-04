#include <iostream>
#include <numeric>
#include "Stats.hpp"
#include "Item.hpp"
#include "Combat_simulator.hpp"
#include "Character.hpp"
#include "Enchant.hpp"

// TODO support skill 300-305
// TODO buffs
// TODO crusader enchant / chance on hit
// TODO stat weights
// TODO stances
// TODO move battle shout in the simulation / double check AP value
// TODO anger management
// TODO deep wounds
// TODO cooldowns
// TODO execute
// TODO overpower
// TODO setbonuses

struct Armory
{
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
    armory.set_extra_item_properties();

    character.equip_armor(armory.lionheart_helm,
                          armory.onyxia_tooth_pendant,
                          armory.truestrike_shoulders,
                          armory.cape_of_the_black_baron,
                          armory.savage_gladiator_chain,
                          armory.wristguards_of_stability,
                          armory.devilsaur_gauntlets,
//                          armory.edgemasters_handguards,
                          armory.onslaught_girdle,
                          armory.devilsaur_leggings,
                          armory.windreaver_greaves,
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
                           Enchant{Enchant::Socket::legs, Enchant::Type::haste}
    );

    character.compute_all_stats(Character::Talent::fury);
    std::cout << character.get_total_special_stats() << "\n";
    std::cout << "haste: " << character.get_haste() << "\n";
    std::cout << "chance for extra hit: " << character.get_chance_for_extra_hit() << "%" << "\n";
    std::cout << character.get_stats() << "\n";

    Combat_simulator combat_simulator;
    combat_simulator.enable_spell_rotation();
    combat_simulator.enable_talents();
    combat_simulator.enable_item_change_on_hit_effects();

    auto dps_snapshots = combat_simulator.simulate(character, 1000, .01, 63);
    double average = std::accumulate(dps_snapshots.begin(), dps_snapshots.end(), 0.0) / dps_snapshots.size();
    std::cout << "DPS from simulation: \n" << average << "\n";
    std::cout << "Average DPS over the simulation: \n";
    for (const auto &elem : dps_snapshots)
    {
        std::cout << elem << ", ";
    }

//    auto dps_snapshots = combat_simulator.simulate(character, 100000, .01, 63);

    return 0;
}

















