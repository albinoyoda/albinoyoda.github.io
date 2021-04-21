#include "Armory.hpp"
#include "BinomialDistribution.hpp"
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

double geometric_series(double p)
{
    return 1 / (1 - p);
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

TEST(TestSuite, test_hit_effects_extra_hit)
{
    auto config = get_test_config();
    config.sim_time = 1000.0;
    config.n_batches = 500.0;

    auto character = get_test_character();
    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    double mh_proc_prob = 0.1;
    double oh_proc_prob = 0.2;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::extra_hit, {}, {}, 0, 0, mh_proc_prob, 0};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::extra_hit, {}, {}, 0, 0, oh_proc_prob, 0};
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();

    double miss_chance = (8 * 0.8 + 20.0) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);

    double expected_swings_oh = config.n_batches * config.sim_time / character.weapons[1].swing_speed;
    BinomialDistribution bin_dist_oh{expected_swings_oh, hit_chance * oh_proc_prob};
    double expected_procs_oh = bin_dist_oh.mean_;
    double conf_interval_oh = bin_dist_oh.confidence_interval_width(0.99);

    double expected_swings_mh = config.n_batches * config.sim_time / character.weapons[0].swing_speed;
    BinomialDistribution bin_dist_mh{expected_swings_mh, hit_chance * mh_proc_prob};
    double expected_procs_mh = bin_dist_mh.mean_;
    auto conf_interval_mh = bin_dist_mh.confidence_interval_width(0.99);

    // OH proc's trigger main hand swings
    expected_swings_mh += expected_procs_mh + expected_procs_oh;

    EXPECT_NEAR(sources.white_mh_count, expected_swings_mh, 0.01 * expected_swings_mh);
    EXPECT_NEAR(sources.white_oh_count, expected_swings_oh, 0.01 * expected_swings_oh);

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, conf_interval_mh / 2);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, conf_interval_oh / 2);
}

TEST(TestSuite, test_hit_effects_physical_damage)
{
    auto config = get_test_config();
    config.sim_time = 100000.0;
    config.n_batches = 1.0;

    auto character = get_test_character();
    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    double mh_proc_prob = 0.1;
    double oh_proc_prob = 0.2;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::damage_physical, {}, {}, 100, 0, mh_proc_prob, 0};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::damage_physical, {}, {}, 100, 0, oh_proc_prob, 0};
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();

    double miss_chance = (8 * 0.8 + 20.0) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);
    double yellow_hit_chance = (1 - 0.09);

    double expected_swings_oh = config.n_batches * config.sim_time / character.weapons[1].swing_speed;
    BinomialDistribution bin_dist_oh{expected_swings_oh, hit_chance * oh_proc_prob};
    double expected_procs_oh = bin_dist_oh.mean_;

    double expected_swings_mh = config.n_batches * config.sim_time / character.weapons[0].swing_speed;
    BinomialDistribution bin_dist_mh{expected_swings_mh, hit_chance * mh_proc_prob};
    double expected_procs_mh = bin_dist_mh.mean_;

    double second_order_procs =
        (expected_procs_oh + expected_procs_mh) * (geometric_series(yellow_hit_chance * mh_proc_prob) - 1);
    expected_procs_mh += second_order_procs;

    double expected_total_procs = expected_procs_oh + expected_procs_mh;
    EXPECT_NEAR(sources.white_mh_count, expected_swings_mh, 0.01 * expected_swings_mh);
    EXPECT_NEAR(sources.white_oh_count, expected_swings_oh, 0.01 * expected_swings_oh);

    EXPECT_NEAR(sources.item_hit_effects_count, expected_total_procs, 0.03 * expected_total_procs);

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, 0.03 * expected_procs_mh);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, 0.03 * expected_procs_oh);
}

TEST(TestSuite, test_hit_effects_magic_damage)
{
    auto config = get_test_config();
    config.sim_time = 1000.0;
    config.n_batches = 100.0;

    auto character = get_test_character();
    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    double mh_proc_prob = 0.1;
    double oh_proc_prob = 0.2;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::damage_magic, {}, {}, 100, 0, mh_proc_prob, 0};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::damage_magic, {}, {}, 100, 0, oh_proc_prob, 0};
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();

    double miss_chance = (8 * 0.8 + 20.0) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);

    double expected_swings_oh = config.n_batches * config.sim_time / character.weapons[1].swing_speed;
    BinomialDistribution bin_dist_oh{expected_swings_oh, hit_chance * oh_proc_prob};
    double expected_procs_oh = bin_dist_oh.mean_;
    double conf_interval_oh = bin_dist_oh.confidence_interval_width(0.99);

    double expected_swings_mh = config.n_batches * config.sim_time / character.weapons[0].swing_speed;
    BinomialDistribution bin_dist_mh{expected_swings_mh, hit_chance * mh_proc_prob};
    double expected_procs_mh = bin_dist_mh.mean_;
    double conf_interval_mh = bin_dist_mh.confidence_interval_width(0.99);

    double expected_total_procs = expected_procs_oh + expected_procs_mh;
    EXPECT_NEAR(sources.white_mh_count, expected_swings_mh, 0.01 * expected_swings_mh);
    EXPECT_NEAR(sources.white_oh_count, expected_swings_oh, 0.01 * expected_swings_oh);

    EXPECT_NEAR(sources.item_hit_effects_count, expected_total_procs, 0.03 * expected_total_procs);

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, conf_interval_mh / 2);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, conf_interval_oh / 2);
}

TEST(TestSuite, test_hit_effects_stat_boost_short_duration)
{
    auto config = get_test_config();
    config.sim_time = 1000.0;
    config.n_batches = 100.0;
    config.performance_mode = false;

    auto character = get_test_character();

    double mh_proc_prob = 1.0;
    int mh_proc_duration = 1;
    double oh_proc_prob = 1.0;
    int oh_proc_duration = 2;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::stat_boost, {50, 0}, {}, 0, mh_proc_duration,
                              mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::stat_boost, {}, {10, 0, 0, 0, 0.1}, 0, oh_proc_duration,
                              oh_proc_prob};
    character.weapons[0].swing_speed = 3.0;
    character.weapons[1].swing_speed = 3.0;
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();
    auto aura_uptimes = sim.get_aura_uptimes_map();

    double miss_chance = (8 * 0.8 + 20.0) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);

    double expected_procs_oh = hit_chance * sources.white_oh_count * oh_proc_prob;
    double expected_uptime_oh = expected_procs_oh * oh_proc_duration;

    double expected_procs_mh = hit_chance * sources.white_mh_count * mh_proc_prob;
    double expected_uptime_mh = expected_procs_mh * mh_proc_duration;

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, 0.03 * expected_procs_mh);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, 0.03 * expected_procs_oh);

    EXPECT_NEAR(aura_uptimes["main_hand_test_wep_mh"], expected_uptime_mh, 0.03 * expected_uptime_mh);
    EXPECT_NEAR(aura_uptimes["off_hand_test_wep_oh"], expected_uptime_oh, 0.03 * expected_uptime_oh);
    EXPECT_TRUE(aura_uptimes["off_hand_test_wep_oh"] != aura_uptimes["off_hand_test_wep_mh"]);
}

TEST(TestSuite, test_hit_effects_stat_boost_long_duration)
{
    auto config = get_test_config();
    config.sim_time = 10000.0;
    config.n_batches = 100.0;
    config.performance_mode = false;

    auto character = get_test_character();

    // Small chance on hit to decrease second order terms, i.e., procing stat buff while a stat buff is already active
    double mh_proc_prob = .01;
    int mh_proc_duration = 30;
    double oh_proc_prob = .01;
    int oh_proc_duration = 20;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::stat_boost, {50, 0}, {}, 0, mh_proc_duration,
                              mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::stat_boost, {}, {10, 0, 0, 0, 0.1}, 0, oh_proc_duration,
                              oh_proc_prob};
    character.weapons[0].swing_speed = 1.7;
    character.weapons[1].swing_speed = 1.9;
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();
    auto aura_uptimes = sim.get_aura_uptimes_map();

    double miss_chance = (8 * 0.8 + 20.0) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);

    double expected_procs_oh = hit_chance * sources.white_oh_count * oh_proc_prob;
    double expected_uptime_oh = expected_procs_oh * oh_proc_duration;
    double swings_during_uptime_oh = oh_proc_duration / character.weapons[1].swing_speed;
    double procs_during_uptime_oh = hit_chance * swings_during_uptime_oh * oh_proc_prob;
    double expected_procs_during_uptime_oh = expected_procs_oh * procs_during_uptime_oh;
    double overlap_duration_oh = expected_procs_during_uptime_oh * oh_proc_duration / 2;

    double expected_procs_mh = hit_chance * sources.white_mh_count * mh_proc_prob;
    double expected_uptime_mh = expected_procs_mh * mh_proc_duration;
    double swings_during_uptime_mh = mh_proc_duration / character.weapons[0].swing_speed;
    double procs_during_uptime_mh = hit_chance * swings_during_uptime_mh * mh_proc_prob;
    double expected_procs_during_uptime_mh = expected_procs_mh * procs_during_uptime_mh;
    double overlap_duration_mh = expected_procs_during_uptime_mh * mh_proc_duration / 2;

    // Does not factor in that a buff might end due to the simulation time reaching its end
    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, 0.03 * expected_procs_mh);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, 0.03 * expected_procs_oh);

    EXPECT_NEAR(aura_uptimes["main_hand_test_wep_mh"], expected_uptime_mh - overlap_duration_mh,
                0.03 * expected_uptime_mh);
    EXPECT_NEAR(aura_uptimes["off_hand_test_wep_oh"], expected_uptime_oh - overlap_duration_oh,
                0.03 * expected_uptime_oh);
    EXPECT_TRUE(aura_uptimes["off_hand_test_wep_oh"] != aura_uptimes["off_hand_test_wep_mh"]);
}

TEST(TestSuite, test_hit_effects_stat_boost_long_duration_overlap)
{
    auto config = get_test_config();
    config.sim_time = 1000.0;
    config.n_batches = 100.0;
    config.performance_mode = false;

    auto character = get_test_character();

    // Small chance on hit to decrease second order terms, i.e., procing stat buff while a stat buff is already active
    double mh_proc_prob = .95;
    int mh_proc_duration = 17;
    double oh_proc_prob = .85;
    int oh_proc_duration = 19;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::stat_boost, {50, 0}, {}, 0, mh_proc_duration,
                              mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::stat_boost, {}, {10, 0, 0, 0, 0.1}, 0, oh_proc_duration,
                              oh_proc_prob};
    character.weapons[0].swing_speed = 1.7;
    character.weapons[1].swing_speed = 1.9;
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    auto aura_uptimes = sim.get_aura_uptimes_map();

    // Subtract 2 seconds since it might not always proc on the first hit
    double expected_duration = (config.sim_time - 2.0) * config.n_batches;

    EXPECT_GT(aura_uptimes["main_hand_test_wep_mh"], 0.98 * expected_duration);
    EXPECT_GT(aura_uptimes["off_hand_test_wep_oh"], 0.98 * expected_duration);
}
