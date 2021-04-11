#include "Helper_functions.hpp"
#include "sim_input_mult.hpp"

#include "gtest/gtest.h"

TEST(TestSuite, test_find_value_class)
{
    std::vector<std::string> mult_armor_vec;
    mult_armor_vec.emplace_back("lionheart_helm");
    mult_armor_vec.emplace_back("onyxia_tooth_pendant");
    mult_armor_vec.emplace_back("truestrike_shoulders");
    mult_armor_vec.emplace_back("cape_of_the_black_baron");
    mult_armor_vec.emplace_back("savage_gladiator_chain");
    mult_armor_vec.emplace_back("wristguards_of_stability");
    mult_armor_vec.emplace_back("flameguard_gauntlets");
    mult_armor_vec.emplace_back("onslaught_girdle");
    mult_armor_vec.emplace_back("cloudkeeper_legplates");
    mult_armor_vec.emplace_back("chromatic_boots");

    {
        std::vector<size_t> values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        Find_values<size_t> fv(mult_armor_vec, values);
        EXPECT_TRUE(fv.find("lionheart_helm") == 0);
        EXPECT_TRUE(fv.find("onyxia_tooth_pendant") == 1);
        EXPECT_TRUE(fv.find("truestrike_shoulders") == 2);
        EXPECT_TRUE(fv.find("cape_of_the_black_baron") == 3);
        EXPECT_TRUE(fv.find("savage_gladiator_chain") == 4);
        EXPECT_TRUE(fv.find("wristguards_of_stability") == 5);
        EXPECT_TRUE(fv.find("flameguard_gauntlets") == 6);
        EXPECT_TRUE(fv.find("onslaught_girdle") == 7);
        EXPECT_TRUE(fv.find("cloudkeeper_legplates") == 8);
        EXPECT_TRUE(fv.find("chromatic_boots") == 9);
    }
    {
        std::vector<double> values{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
        Find_values<double> fv(mult_armor_vec, values);
        EXPECT_TRUE(fv.find("lionheart_helm") == 0.0);
        EXPECT_TRUE(fv.find("onyxia_tooth_pendant") == 1.0);
        EXPECT_TRUE(fv.find("truestrike_shoulders") == 2.0);
        EXPECT_TRUE(fv.find("cape_of_the_black_baron") == 3.0);
        EXPECT_TRUE(fv.find("savage_gladiator_chain") == 4.0);
        EXPECT_TRUE(fv.find("wristguards_of_stability") == 5.0);
        EXPECT_TRUE(fv.find("flameguard_gauntlets") == 6.0);
        EXPECT_TRUE(fv.find("onslaught_girdle") == 7.0);
        EXPECT_TRUE(fv.find("cloudkeeper_legplates") == 8.0);
        EXPECT_TRUE(fv.find("chromatic_boots") == 9.0);
    }
}
