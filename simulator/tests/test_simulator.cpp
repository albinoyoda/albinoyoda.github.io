#include "Armory.hpp"
#include "simulation_fixture.cpp"
#include "simulator/combat_simulator.hpp"
#include "statistics/BinomialDistribution.hpp"
#include "statistics/Statistics.hpp"

/// Test enabling different use effects and checking uptime
/// test equip effects: HoJ, servo arm

TEST_F(Sim_fixture, test_no_crit_equals_no_flurry_uptime)
{
    config.talents.flurry = 5;
    config.sim_time = Sim_time::from_seconds(500);
    config.main_target_level = 60;

    character.total_special_stats.critical_strike = 0;

    sim.set_config(config);
    sim.simulate(character);

    EXPECT_EQ(sim.get_flurry_uptime_mh(), 0.0);
    EXPECT_EQ(sim.get_flurry_uptime_oh(), 0.0);
}

TEST_F(Sim_fixture, test_some_crit_equals_some_flurry_uptime)
{
    config.talents.flurry = 5;
    config.sim_time = Sim_time::from_seconds(500);
    config.main_target_level = 60;

    character.total_special_stats.critical_strike = 20;

    sim.set_config(config);
    sim.simulate(character);

    EXPECT_GT(sim.get_flurry_uptime_mh(), 20.0);
    EXPECT_LT(sim.get_flurry_uptime_mh(), 90.0);
    EXPECT_GT(sim.get_flurry_uptime_oh(), 20.0);
    EXPECT_LT(sim.get_flurry_uptime_oh(), 90.0);
}

TEST_F(Sim_fixture, test_max_crit_equals_high_flurry_uptime)
{
    config.talents.flurry = 5;
    config.sim_time = Sim_time::from_seconds(500);
    config.main_target_level = 60;

    character.total_special_stats.critical_strike = 100;

    sim.set_config(config);
    sim.simulate(character);

    EXPECT_GT(sim.get_flurry_uptime_mh(), .95);
    EXPECT_GT(sim.get_flurry_uptime_oh(), .95);
    EXPECT_NEAR(sim.get_flurry_uptime_mh(), sim.get_flurry_uptime_oh(), 0.02);
    EXPECT_NE(sim.get_flurry_uptime_mh(), sim.get_flurry_uptime_oh());
}

TEST_F(Sim_fixture, test_white_hits_count)
{
    sim.set_config(config);
    sim.simulate(character);

    auto distrib = sim.get_damage_distribution();

    auto expected_mh_swings{config.n_batches * config.sim_time / character.weapons[0].swing_speed};
    auto expected_oh_swings{config.n_batches * config.sim_time / character.weapons[1].swing_speed};

    EXPECT_NEAR(distrib.white_mh_count, expected_mh_swings, 1.0);
    EXPECT_NEAR(distrib.white_oh_count, expected_oh_swings, 1.0);
}

TEST_F(Sim_fixture, test_bloodthirst_count)
{
    // TODO: parametrize over different spells
    config.talents.bloodthirst = 1;
    config.combat.use_bloodthirst = true;
    config.essence_of_the_red_ = true;
    config.combat.initial_rage = 100;

    character.total_special_stats.attack_power = 10000;

    sim.set_config(config);
    sim.simulate(character);

    auto distrib = sim.get_damage_distribution();

    EXPECT_NEAR(distrib.bloodthirst_count, config.sim_time.seconds() / 6.0 * config.n_batches, 1.0);
}

TEST_F(Sim_fixture, test_that_with_infinite_rage_all_hits_are_heroic_strike)
{
    config.essence_of_the_red_ = true;
    config.combat.initial_rage = 100;
    config.combat.first_hit_heroic_strike = true;
    config.combat.use_heroic_strike = true;
    config.talents.improved_heroic_strike = 3;
    config.sim_time = Sim_time::from_seconds(500);

    character.total_special_stats.attack_power = 10000;
    character.weapons[0].swing_speed = Sim_time::from_seconds(1.9);
    character.weapons[1].swing_speed = Sim_time::from_seconds(1.7);

    sim.set_config(config);
    sim.simulate(character);

    auto distrib = sim.get_damage_distribution();
    auto hs_uptime = sim.get_hs_uptime();
    double expected_swings_per_simulation =
        (config.sim_time - Sim_time::from_seconds(1)) / character.weapons[0].swing_speed + 1;
    double tolerance = 1.0 / character.weapons[0].swing_speed.seconds();
    EXPECT_NEAR(distrib.heroic_strike_count / double(config.n_batches), expected_swings_per_simulation, tolerance);

    // subtract 'config.n_batches' since each simulation starts with both weapons swinging, which means that one hit per
    // simulation cant be HS influenced
    double hs_uptime_expected = double(distrib.white_oh_count - config.n_batches) / distrib.white_oh_count;
    EXPECT_FLOAT_EQ(hs_uptime, hs_uptime_expected);
}

TEST_F(Sim_fixture, test_simulator_sets_hit_tables)
{
    config.main_target_level = 63;
    config.n_batches = 0;
    character.total_special_stats.critical_strike = 0;
    sim.set_config(config);
    sim.simulate(character);

    Hit_tables hit_tables{config.main_target_level, config.talents.overpower, config.talents.impale};
    hit_tables.compute_hit_table(300, character.total_special_stats, Socket::main_hand, Weapon_socket::main_hand);
    hit_tables.compute_hit_table(300, character.total_special_stats, Socket::off_hand, Weapon_socket::off_hand);

    EXPECT_EQ(hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::miss),
              hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::miss));

    EXPECT_EQ(hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::dodge),
              hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::dodge));

    EXPECT_EQ(hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::glancing),
              hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::glancing));

    EXPECT_EQ(hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::crit),
              hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::crit));

    EXPECT_EQ(hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::hit),
              hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::hit));
}

TEST_F(Sim_fixture, test_dps_return_matches_heuristic_values)
{
    config.main_target_initial_armor_ = 0.0;
    config.sim_time = Sim_time::from_seconds(1000);
    config.n_batches = 500.0;

    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    sim.set_config(config);
    sim.simulate(character);

    double miss_chance = (8 * 0.8 + 20.0) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double glancing_chance = 0.4;
    double hit_chance = (1 - dodge_chance - miss_chance - glancing_chance);

    const auto& mh_wep{character.weapons[0]};
    auto mh_dps{(mh_wep.max_damage + mh_wep.min_damage) / 2 / mh_wep.swing_speed.seconds()};
    double dps_white = 1.5 * mh_dps;
    double expected_dps = dps_white * hit_chance + dps_white * 0.65 * glancing_chance;

    Distribution distribution = sim.get_dps_distribution();
    auto conf_interval = distribution.confidence_interval_of_the_mean(0.99);

    EXPECT_LT(conf_interval.first, expected_dps);
    EXPECT_GT(conf_interval.second, expected_dps);
}

TEST_F(Sim_fixture, test_hit_effects_extra_hit)
{
    config.sim_time = Sim_time::from_seconds(1000);
    config.n_batches = 500.0;

    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    double mh_proc_prob = 0.1;
    double oh_proc_prob = 0.2;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::extra_hit, {}, {}, 0, Sim_time::zero(), mh_proc_prob, 0};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::extra_hit, {}, {}, 0, Sim_time::zero(), oh_proc_prob, 0};
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

TEST_F(Sim_fixture, test_hit_effects_physical_damage)
{
    config.sim_time = Sim_time::from_seconds(100000);
    config.n_batches = 1;
    config.execute_phase_percentage_ = 0.0;

    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    double mh_proc_prob = 0.1;
    double oh_proc_prob = 0.2;
    Hit_effect test_effect_mh{
        "test_wep_mh", Hit_effect::Type::damage_physical, {}, {}, 100, Sim_time::zero(), mh_proc_prob, 0};
    Hit_effect test_effect_oh{
        "test_wep_oh", Hit_effect::Type::damage_physical, {}, {}, 100, Sim_time::zero(), oh_proc_prob, 0};
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
    double yellow_hit_chance = (1 - 0.09 - 0.065);

    double expected_swings_oh = config.n_batches * config.sim_time / character.weapons[1].swing_speed;
    BinomialDistribution bin_dist_oh{expected_swings_oh, hit_chance * oh_proc_prob};
    double expected_procs_oh = bin_dist_oh.mean_;

    double expected_swings_mh = config.n_batches * config.sim_time / character.weapons[0].swing_speed;
    BinomialDistribution bin_dist_mh{expected_swings_mh, hit_chance * mh_proc_prob};
    double expected_procs_mh = bin_dist_mh.mean_;

    double second_order_procs =
        (expected_procs_oh + expected_procs_mh) * (Statistics::geometric_series(yellow_hit_chance * mh_proc_prob) - 1);
    expected_procs_mh += second_order_procs;

    double expected_total_procs = expected_procs_oh + expected_procs_mh;
    EXPECT_NEAR(sources.white_mh_count, expected_swings_mh, 0.02 * expected_swings_mh);
    EXPECT_NEAR(sources.white_oh_count, expected_swings_oh, 0.02 * expected_swings_oh);

    EXPECT_NEAR(sources.item_hit_effects_count, expected_total_procs, 0.02 * expected_total_procs);

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, 0.02 * expected_procs_mh);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, 0.02 * expected_procs_oh);
}

TEST_F(Sim_fixture, test_hit_effects_magic_damage)
{
    config.sim_time = Sim_time::from_seconds(1000);
    config.n_batches = 100.0;

    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    double mh_proc_prob = 0.1;
    double oh_proc_prob = 0.2;
    Hit_effect test_effect_mh{
        "test_wep_mh", Hit_effect::Type::damage_magic, {}, {}, 100, Sim_time::zero(), mh_proc_prob, 0};
    Hit_effect test_effect_oh{
        "test_wep_oh", Hit_effect::Type::damage_magic, {}, {}, 100, Sim_time::zero(), oh_proc_prob, 0};
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

TEST_F(Sim_fixture, test_hit_effects_stat_boost_short_duration)
{
    config.sim_time = Sim_time::from_seconds(1000);
    config.n_batches = 100.0;
    config.performance_mode = false;

    double mh_proc_prob = 1.0;
    auto mh_proc_duration{Sim_time::from_seconds(1)};
    double oh_proc_prob = 1.0;
    auto oh_proc_duration{Sim_time::from_seconds(2)};
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::stat_boost, {50, 0}, {}, 0, mh_proc_duration,
                              mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::stat_boost, {}, {10, 0, 0, 0, 0.1}, 0, oh_proc_duration,
                              oh_proc_prob};
    character.weapons[0].swing_speed = Sim_time::from_seconds(3.0);
    character.weapons[1].swing_speed = Sim_time::from_seconds(3.0);
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
    Sim_time expected_uptime_oh = expected_procs_oh * oh_proc_duration;

    double expected_procs_mh = hit_chance * sources.white_mh_count * mh_proc_prob;
    Sim_time expected_uptime_mh = expected_procs_mh * mh_proc_duration;

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, 0.03 * expected_procs_mh);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, 0.03 * expected_procs_oh);

    EXPECT_NEAR(aura_uptimes["main_hand_test_wep_mh"].seconds(), expected_uptime_mh.seconds(),
                0.03 * expected_uptime_mh.seconds());
    EXPECT_NEAR(aura_uptimes["off_hand_test_wep_oh"].seconds(), expected_uptime_oh.seconds(),
                0.03 * expected_uptime_oh.seconds());
    EXPECT_TRUE(aura_uptimes["off_hand_test_wep_oh"].seconds() != aura_uptimes["off_hand_test_wep_mh"].seconds());
}

TEST_F(Sim_fixture, test_hit_effects_stat_boost_long_duration)
{
    config.sim_time = Sim_time::from_seconds(10000);
    config.n_batches = 100.0;
    config.performance_mode = false;

    // Small chance on hit to decrease second order terms, i.e., procing stat buff while a stat buff is already active
    double mh_proc_prob = .01;
    auto mh_proc_duration{Sim_time::from_seconds(30)};
    double oh_proc_prob = .01;
    auto oh_proc_duration{Sim_time::from_seconds(20)};
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::stat_boost, {50, 0}, {}, 0, mh_proc_duration,
                              mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::stat_boost, {}, {10, 0, 0, 0, 0.1}, 0, oh_proc_duration,
                              oh_proc_prob};
    character.weapons[0].swing_speed = Sim_time::from_seconds(1.7);
    character.weapons[1].swing_speed = Sim_time::from_seconds(1.9);
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
    Sim_time expected_uptime_oh = expected_procs_oh * oh_proc_duration;
    double swings_during_uptime_oh = oh_proc_duration / character.weapons[1].swing_speed;
    double procs_during_uptime_oh = hit_chance * swings_during_uptime_oh * oh_proc_prob;
    double expected_procs_during_uptime_oh = expected_procs_oh * procs_during_uptime_oh;
    Sim_time overlap_duration_oh = expected_procs_during_uptime_oh * oh_proc_duration / 2;

    double expected_procs_mh = hit_chance * sources.white_mh_count * mh_proc_prob;
    Sim_time expected_uptime_mh = expected_procs_mh * mh_proc_duration;
    double swings_during_uptime_mh = mh_proc_duration / character.weapons[0].swing_speed;
    double procs_during_uptime_mh = hit_chance * swings_during_uptime_mh * mh_proc_prob;
    double expected_procs_during_uptime_mh = expected_procs_mh * procs_during_uptime_mh;
    Sim_time overlap_duration_mh = expected_procs_during_uptime_mh * mh_proc_duration / 2;

    // Does not factor in that a buff might end due to the simulation time reaching its end
    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, 0.05 * expected_procs_mh);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, 0.05 * expected_procs_oh);

    EXPECT_NEAR(aura_uptimes["main_hand_test_wep_mh"].seconds(), (expected_uptime_mh - overlap_duration_mh).seconds(),
                0.05 * expected_uptime_mh.seconds());
    EXPECT_NEAR(aura_uptimes["off_hand_test_wep_oh"].seconds(), (expected_uptime_oh - overlap_duration_oh).seconds(),
                0.05 * expected_uptime_oh.seconds());
    EXPECT_TRUE(aura_uptimes["off_hand_test_wep_oh"].seconds() != aura_uptimes["off_hand_test_wep_mh"].seconds());
}

TEST_F(Sim_fixture, test_hit_effects_stat_boost_long_duration_overlap)
{
    config.sim_time = Sim_time::from_seconds(1000);
    config.n_batches = 100.0;
    config.performance_mode = false;

    // Small chance on hit to decrease second order terms, i.e., procing stat buff while a stat buff is already active
    double mh_proc_prob = .95;
    auto mh_proc_duration{Sim_time::from_seconds(17)};
    double oh_proc_prob = .85;
    auto oh_proc_duration{Sim_time::from_seconds(19)};
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::stat_boost, {50, 0}, {}, 0, mh_proc_duration,
                              mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::stat_boost, {}, {10, 0, 0, 0, 0.1}, 0, oh_proc_duration,
                              oh_proc_prob};
    character.weapons[0].swing_speed = Sim_time::from_seconds(1.7);
    character.weapons[1].swing_speed = Sim_time::from_seconds(1.9);
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    auto aura_uptimes = sim.get_aura_uptimes_map();

    // Subtract 2 seconds since it might not always proc on the first hit
    auto expected_duration = (config.sim_time - Sim_time::from_seconds(2)) * config.n_batches;

    EXPECT_GT(aura_uptimes["main_hand_test_wep_mh"], 0.98 * expected_duration);
    EXPECT_GT(aura_uptimes["off_hand_test_wep_oh"], 0.98 * expected_duration);
}
