#include <iostream>
#include <cstdlib>
#include <vector>
#include <cassert>
#include "Stats.hpp"
#include "Item.hpp"
#include "Combat_simulator.hpp"
#include "Character.hpp"

int main()
{
    Character character{Race::human};

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
    Armor satyrs_bow = Armor{Stats{0, 3}, Special_stats{0, 1, 0}, Armor::Socket::trinket2};

    Weapon Brutality_blade = Weapon{2.5, {90, 168}, Stats{9, 9}, Special_stats{1, 0, 0}, Weapon::Socket::main_hand};
    Weapon Mirahs_song = Weapon{1.8, {57, 87}, Stats{9, 9}, Special_stats{0, 0, 0}, Weapon::Socket::off_hand};

    character.equip_armor(lionheart_helm,
                          onyxia_tooth_pendant,
                          truestrike_shoulders,
                          cape_of_the_black_baron,
                          savage_gladiator_chain,
                          wristguards_of_stability,
                          devilsaur_gauntlets,
                          onslaught_girdle,
                          devilsaur_leggings,
                          windreaver_greaves,
                          don_julios_band,
                          magnis_will,
                          hand_of_justice,
                          blackhands_breadth,
                          satyrs_bow);
    character.equip_weapon(Brutality_blade, Mirahs_song);
    character.compute_special_stats();

    Combat_simulator combat_simulator;

    double dps = combat_simulator.simulate(character, 100, 0.01, 63);

    std::cout << "Dps from simulation: " << dps << "\n";

    return 0;
}

















