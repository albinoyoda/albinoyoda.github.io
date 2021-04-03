#include "Attributes.hpp"

#include "gtest/gtest.h"

TEST(TestSuite, test_attributes)
{
    const auto a = Attributes{5, 10};
    const auto b = Attributes{15, 20};
    const auto c = a + b;

    ASSERT_FLOAT_EQ(c.strength, 20);
    ASSERT_FLOAT_EQ(c.agility, 30);

    auto st = Special_stats{};
    st.stat_multiplier = 0.5;

    auto multiplied = a.multiply(st);

    ASSERT_FLOAT_EQ(multiplied.strength, 7.5);
    ASSERT_FLOAT_EQ(multiplied.agility, 15);

    st = b.convert_to_special_stats(st);

    ASSERT_FLOAT_EQ(st.attack_power, 45.0);
    ASSERT_FLOAT_EQ(st.critical_strike, 1.5);
}