#include <iostream>
#include "Stats.hpp"
#include "Item.hpp"
#include "Combat_simulator.hpp"
#include "Character.hpp"

struct Armory
{
    Armor lionheart_helm = Armor{Stats{18, 0}, Special_stats{2, 2, 0}, Armor::Socket::head};
    Armor onyxia_tooth_pendant = Armor{Stats{0, 13}, Special_stats{1, 1, 0}, Armor::Socket::neck};
    Armor truestrike_shoulders = Armor{Stats{0, 0}, Special_stats{0, 2, 24}, Armor::Socket::shoulder};
    Armor cape_of_the_black_baron = Armor{Stats{0, 15}, Special_stats{0, 0, 20}, Armor::Socket::back};
    Armor savage_gladiator_chain = Armor{Stats{13, 14}, Special_stats{2, 0, 0}, Armor::Socket::chest};
    Armor wristguards_of_stability = Armor{Stats{24, 0}, Special_stats{0, 0, 0}, Armor::Socket::wrists};
    Armor devilsaur_gauntlets = Armor{Stats{0, 0}, Special_stats{1, 1, 28}, Armor::Socket::hands};
    Armor onslaught_girdle = Armor{Stats{31, 0}, Special_stats{1, 1, 0}, Armor::Socket::belt};
    Armor devilsaur_leggings = Armor{Stats{0, 0}, Special_stats{1, 1, 46}, Armor::Socket::legs};
    Armor windreaver_greaves = Armor{Stats{0, 20}, Special_stats{0, 1, 0}, Armor::Socket::boots};
    Armor andra_bootsen = Armor{Stats{9, 9}, Special_stats{0, 1, 0}, Armor::Socket::boots};
    Armor don_julios_band = Armor{Stats{0, 0}, Special_stats{1, 1, 16}, Armor::Socket::ring1};
    Armor magnis_will = Armor{Stats{6, 0}, Special_stats{1, 0, 0}, Armor::Socket::ring2};
    Armor hand_of_justice = Armor{Stats{0, 0}, Special_stats{0, 0, 20}, Armor::Socket::trinket1};
    Armor blackhands_breadth = Armor{Stats{0, 0}, Special_stats{2, 0, 0}, Armor::Socket::trinket2};
    Armor satyrs_bow = Armor{Stats{0, 3}, Special_stats{0, 1, 0}, Armor::Socket::ranged};

    Weapon brutality_blade = Weapon{2.5, {90, 168}, Stats{9, 9}, Special_stats{1, 0, 0}, Weapon::Socket::main_hand};
    Weapon mirahs_song = Weapon{1.8, {57, 87}, Stats{9, 9}, Special_stats{0, 0, 0}, Weapon::Socket::off_hand};
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
                          armory.wristguards_of_stability,
                          armory.devilsaur_gauntlets,
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

    character.compute_special_stats();

    Combat_simulator combat_simulator;
    combat_simulator.enable_spell_rotation();
    combat_simulator.enable_talents();
    combat_simulator.enable_item_change_on_hit_effects();

    std::cout << character.get_total_special_stats() << "\n";

    double dps = combat_simulator.simulate(character, 100, 0.01, 63);

    std::cout << "Dps from simulation: " << dps << "\n";

    return 0;
}

















