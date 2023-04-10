#include "Armory.hpp"
#include "simulation_fixture.cpp"
#include "simulator/combat_simulator.hpp"
#include "statistics/BinomialDistribution.hpp"
#include "statistics/Statistics.hpp"

TEST(Hit_tables, test_hit_tables)
{
    // TODO parametrize!!
    Hit_tables hit_tables{63, 0, 0};
    Special_stats ss{};
    hit_tables.compute_hit_table(300, ss, Socket::main_hand, Weapon_socket::main_hand);
    hit_tables.compute_hit_table(300, ss, Socket::off_hand, Weapon_socket::off_hand);

    double expected_miss{8 * 0.8 + 20.0};
    double sim_miss{hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::miss)};
    EXPECT_FLOAT_EQ(sim_miss, expected_miss);

    double expected_dodge{6.5};
    double sim_dodge{hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::dodge)};
    EXPECT_FLOAT_EQ(sim_dodge, expected_dodge);

    double expected_glancing{40.0};
    double sim_glancing{hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::glancing)};
    EXPECT_FLOAT_EQ(sim_glancing, expected_glancing);

    double expected_crit{0.0};
    double sim_crit{hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::crit)};
    EXPECT_FLOAT_EQ(sim_crit, expected_crit);

    double expected_hit{(1 - expected_dodge - expected_miss - expected_glancing - expected_crit)};
    double sim_hit{hit_tables.get_probability(Weapon_socket::main_hand, Hit_color::white, Melee_result::hit)};
    EXPECT_FLOAT_EQ(sim_hit, expected_hit);
}
