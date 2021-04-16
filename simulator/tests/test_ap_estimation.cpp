#include "Armory.hpp"
#include "Combat_simulator.hpp"

#include "gtest/gtest.h"

TEST(TestSuite, test_ap_estimation)
{
    Special_stats special_stats{};
    special_stats.critical_strike = 50;
    special_stats.hit = 12;
    special_stats.attack_power = 2500;
    special_stats.chance_for_extra_hit = 0;
    special_stats.haste = 0;
    special_stats.sword_skill = 311;
    special_stats.axe_skill = 311;
    special_stats.dagger_skill = 311;
    special_stats.mace_skill = 311;
    special_stats.fist_skill = 311;
    special_stats.damage_mod_physical = 0.1;
    special_stats.damage_mod_spell = 0.1;
    special_stats.stat_multiplier = 0.265;
    special_stats.bonus_damage = 10;

    Armory armory;
    auto wep1 = armory.find_weapon(Weapon_socket::one_hand, "thunderfury_blessed_blade");
    auto wep2 = armory.find_weapon(Weapon_socket::one_hand, "brutality_blade");

    Weapon wep3{"test_oh", {10, 10}, {2, 1, 40}, 1.5, 75, 120, Weapon_socket::main_hand, Weapon_type::sword};

    double ap_equiv_1 = get_character_ap_equivalent(special_stats, wep1, wep3, 65.0, {});

    double ap_equiv_2 = get_character_ap_equivalent(special_stats, wep2, wep3, 65.0, {});

    EXPECT_GE(ap_equiv_1, ap_equiv_2);
}