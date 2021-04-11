#include "Armory.hpp"
#include "Combat_simulator.hpp"

#include "gtest/gtest.h"

namespace
{
Combat_simulator_config get_test_config()
{
    Combat_simulator_config config{};

    //    config.exposed_armor = true;
    //    config.curse_of_recklessness_active = true;
    //    config.faerie_fire_feral_active = true;
    //    config.combat.use_death_wish = true;
    //    config.enable_recklessness = true;
    //    config.enable_blood_fury = true;
    //    config.enable_berserking = true;
    //    config.combat.use_bt_in_exec_phase = true;
    //    config.combat.use_ms_in_exec_phase = true;
    //    config.combat.use_hs_in_exec_phase = true;
    //    config.combat.cleave_if_adds = true;
    //    config.combat.use_sweeping_strikes = true;
    //    config.combat.dont_use_hm_when_ss = true;
    //    config.combat.use_hamstring = true;
    //    config.combat.use_bloodthirst = true;
    //    config.combat.use_mortal_strike = true;
    //    config.combat.use_slam = true;
    //    config.combat.use_whirlwind = true;
    //    config.combat.use_overpower = true;
    //    config.combat.use_heroic_strike = true;
    //    config.combat.deep_wounds = true;
    //    config.combat.heroic_strike_damage = 157;
    config.combat.heroic_strike_damage = 138;
    //    config.combat.first_hit_heroic_strike = true;
    //    config.multi_target_mode_ = true;
    //    config.essence_of_the_red_ = true;
    //    config.can_trigger_enrage_ = true;
    //    config.first_global_sunder_ = true;
    //    config.take_periodic_damage_ = true;
    //    config.ability_queue_ = true;

    config.combat.heroic_strike_rage_thresh = 0.0;
    config.combat.cleave_rage_thresh = 0.0;
    config.combat.whirlwind_rage_thresh = 0.0;
    config.combat.hamstring_cd_thresh = 0.0;
    config.combat.hamstring_thresh_dd = 0.0;
    config.combat.initial_rage = 0.0;
    config.combat.whirlwind_bt_cooldown_thresh = 0.0;
    config.combat.overpower_rage_thresh = 0.0;
    config.combat.overpower_bt_cooldown_thresh = 0.0;
    config.combat.overpower_ww_cooldown_thresh = 0.0;
    config.combat.slam_cd_thresh = 0.0;
    config.combat.slam_spam_max_time = 0.0;
    config.combat.slam_spam_rage = 0.0;
    config.combat.slam_rage_dd = 0.0;

    config.sim_time = 60.0;
    config.main_target_level = 63.0;
    config.main_target_initial_armor_ = 3731.0;
    config.n_sunder_armor_stacks = 5.0;
    config.number_of_extra_targets = 0.0;
    config.extra_target_duration = 0.0;
    config.extra_target_initial_armor_ = 0.0;
    config.extra_target_level = 60.0;
    config.periodic_damage_interval_ = 0.0;
    config.periodic_damage_amount_ = 0.0;
    config.execute_phase_percentage_ = 0.0;
    config.ability_queue_rage_thresh_ = 0.0;
    config.berserking_haste_ = 0.0;

    config.talents.improved_heroic_strike = 0;
    config.talents.overpower = 0;
    config.talents.unbridled_wrath = 0;
    config.talents.flurry = 0;
    config.talents.anger_management = false;
    config.talents.impale = 0;
    config.talents.improved_execute = 0;
    config.talents.dual_wield_specialization = 0;
    config.talents.improved_cleave = 0;
    config.talents.improved_slam = 0;
    config.talents.death_wish = false;
    config.talents.tactical_mastery = 0;
    config.talents.deep_wounds = 0;
    config.talents.bloodthirst = 0;
    config.talents.mortal_strike = 0;
    config.talents.sweeping_strikes = 0;

    config.performance_mode = true;
    config.n_batches = 10;

    return config;
}

Character get_test_character()
{
    Character character{Race::gnome, 60};
    Weapon wep = Weapon{"test_wep", {}, {}, 2.0, 100, 100, Weapon_socket::one_hand, Weapon_type::axe};

    character.equip_weapon(wep, wep);

    Armory armory{};
    armory.compute_total_stats(character);

    return character;
}
} // namespace

TEST(TestSuite, test_flurry)
{
    auto config = get_test_config();
    config.talents.flurry = 5;
    config.sim_time = 500.0;
    config.main_target_level = 60;

    auto character = get_test_character();
    character.total_special_stats.critical_strike = 0;

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    EXPECT_EQ(sim.get_flurry_uptime_mh(), 0.0);
    EXPECT_EQ(sim.get_flurry_uptime_oh(), 0.0);

    character.total_special_stats.critical_strike = 100;
    sim.simulate(character);

    EXPECT_GT(sim.get_flurry_uptime_mh(), .95);
    EXPECT_GT(sim.get_flurry_uptime_oh(), .95);
    EXPECT_NEAR(sim.get_flurry_uptime_mh(), sim.get_flurry_uptime_oh(), 0.02);
    EXPECT_NE(sim.get_flurry_uptime_mh(), sim.get_flurry_uptime_oh());
}

TEST(TestSuite, test_bloodthirst_count)
{
    auto config = get_test_config();
    config.talents.bloodthirst = 1;
    config.combat.use_bloodthirst = true;
    config.essence_of_the_red_ = true;
    config.combat.initial_rage = 100;

    auto character = get_test_character();
    character.total_special_stats.attack_power = 10000;

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    auto distrib = sim.get_damage_distribution();

    EXPECT_NEAR(distrib.bloodthirst_count, config.sim_time / 6.0 * config.n_batches, 1.0);
}

TEST(TestSuite, test_heroic_strike)
{
    auto config = get_test_config();
    config.essence_of_the_red_ = true;
    config.combat.initial_rage = 100;
    config.combat.first_hit_heroic_strike = true;
    config.combat.use_heroic_strike = true;
    config.talents.improved_heroic_strike = 3;
    config.sim_time = 500.0;

    auto character = get_test_character();
    character.total_special_stats.attack_power = 10000;
    character.weapons[0].swing_speed = 1.9;
    character.weapons[1].swing_speed = 1.7;

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    auto distrib = sim.get_damage_distribution();
    auto hs_uptime = sim.get_hs_uptime();
    double expected_swings_per_simulation = (config.sim_time - 1.0) / character.weapons[0].swing_speed + 1;
    double tolerance = 1.0 / character.weapons[0].swing_speed;
    EXPECT_NEAR(distrib.heroic_strike_count / double(config.n_batches), expected_swings_per_simulation, tolerance);

    // subtract 'config.n_batches' since each simulation starts with both weapons swinging, which means that one hit per
    // simulation cant be HS influenced
    double hs_uptime_expected = double(distrib.white_oh_count - config.n_batches) / distrib.white_oh_count;
    EXPECT_FLOAT_EQ(hs_uptime, hs_uptime_expected);
}

TEST(TestSuite, test_dps_return)
{
    auto config = get_test_config();
    config.main_target_initial_armor_ = 0.0;
    config.sim_time = 1000.0;
    config.n_batches = 500.0;

    auto character = get_test_character();
    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);
    double miss_chance = (8 * 0.8 + 20.0) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double glancing_chance = 0.4;
    double hit_chance = (1 - dodge_chance - miss_chance - glancing_chance);
    double dps_white = 50 + 25;
    double expected_dps = dps_white * hit_chance + dps_white * 0.65 * glancing_chance;

    Distribution distribution = sim.get_dps_distribution();
    auto conf_interval = distribution.confidence_interval_of_the_mean(0.99);

    EXPECT_LT(conf_interval.first, expected_dps);
    EXPECT_GT(conf_interval.second, expected_dps);
}