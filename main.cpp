#include <iostream>
#include <cstdlib>
#include <vector>
#include <cassert>
#include "Stats.hpp"
#include "Item.hpp"
#include "Combat_simulator.hpp"

enum class Race
{
    human,
};

class Character
{
public:
    explicit Character(const Race &race) : base_stats_{}, total_special_stats_{}, weapon_skill_{300}
    {
        switch (race)
        {
            case Race::human:
                base_stats_ += Stats{120, 80};
                total_special_stats_ += Special_stats{5, 0, 0};
                weapon_skill_ += 5;
                return;
            default:
                assert(false);
        }
    }

    Stats compute_total_stats()
    {
        Stats total_stats = base_stats_;
        for (const Armor &armor : gear)
        {
            total_stats += armor.get_stats();
        }
        for (const Weapon &weapon : weapons)
        {
            total_stats += weapon.get_stats();
        }
        return total_stats;
    };

    void compute_special_stats()
    {
        // TODO do buffs here
        auto total_stats = compute_total_stats();
        for (const Item &armor : gear)
        {
            total_special_stats_ += armor.get_special_stats();
        }
        for (const Item &weapon : gear)
        {
            total_special_stats_ += weapon.get_special_stats();
        }
        total_special_stats_ += total_stats.convert_to_special_stats();
    };

    Combat_simulator compute_hit_table(double level)
    {
        // TODO level based glancing/hit/crit

        double target_defence = level * 5;
        double skill_diff = target_defence - weapon_skill_;

        // Crit chance
        double crit_chance = total_special_stats_.critical_strike - skill_diff * 0.2;

        // Miss chance
        double base_miss_chance = (5 + skill_diff * 0.1);
        double dw_miss_chance = (base_miss_chance * 0.8 + 20);
        double miss_chance = dw_miss_chance - total_special_stats_.hit;

        // Dodge chance
        double dodge_chance = (5 + skill_diff * 0.1);

        // Glancing blows
        double glancing_chance = 40;

        // Order -> Miss, parry, dodge, block, glancing, crit, hit.
        std::vector<double> hit_table = {miss_chance};
        hit_table.push_back(hit_table.back() + dodge_chance);
        hit_table.push_back(hit_table.back() + glancing_chance);
        hit_table.push_back(hit_table.back() + crit_chance);
        if (hit_table.back() > 100)
        {
            std::cout << "crit capped!!" << "\n";
        }

        return Combat_simulator{hit_table};
    };

    double simulate(double sim_time, double dt, int level, int iterations)
    {
        double time = 0;

        Combat_simulator hit_table = compute_hit_table(63);

        double total_damage = 0;

        while (time < sim_time)
        {
            auto damage_main = weapons[0].step(dt, total_special_stats_.attack_power, true);
            auto filtered_damage = hit_table.generate_hit(damage_main);
            total_damage += filtered_damage;

            auto damage_off = weapons[1].step(dt, total_special_stats_.attack_power, false);
            filtered_damage = hit_table.generate_hit(damage_main);
            total_damage += filtered_damage;

            time += dt;
        }

        return total_damage / sim_time;
    }

    template<typename T, typename ...Ts>
    void equip_armor(T piece, Ts ...pieces)
    {
        equip_armor(piece);
        equip_armor(pieces...);
    }

    template<typename T>
    void equip_armor(T piece)
    {
        gear.emplace_back(piece);
    }

    template<typename T, typename ...Ts>
    void equip_weapon(T piece, Ts ...pieces)
    {
        equip_weapon(piece);
        equip_weapon(pieces...);
    }

    template<typename T>
    void equip_weapon(T piece)
    {
        weapons.emplace_back(piece);
    }

private:
    Stats base_stats_;
    Special_stats total_special_stats_;
    double weapon_skill_;

    std::vector<Armor> gear;
    std::vector<Weapon> weapons;
};


int main()
{
    Character character{Race::human};

//    std::vector<std::string> gear{"Lionheart_helm", "Onyxia_tooth_pendant"};
//    std::vector<std::string> weapons{"Brutality_blade", "Mirahs_song"};

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
    double dps = character.simulate(60, 0.1, 63, 1);

    std::cout << "Dps from simulation: " << dps << "\n";

    return 0;
}

















