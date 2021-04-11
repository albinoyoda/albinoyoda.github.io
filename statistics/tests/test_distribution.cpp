#include "Distribution.hpp"
#include "Statistics.hpp"

#include "gtest/gtest.h"
#include <random>

TEST(TestSuite, test_distribution)
{
    std::vector<double> data = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    double population_std = 2.5820; // taken from wolfram alpha

    Distribution distribution{};
    for (const auto& sample : data)
    {
        distribution.add_sample(sample);
    }
    ASSERT_DOUBLE_EQ(distribution.mean_, 5);
    EXPECT_NEAR(distribution.std_(), population_std, 0.01);
    EXPECT_NEAR(distribution.variance_, population_std * population_std, 0.01);
    EXPECT_NEAR(distribution.std_of_the_mean(), population_std / 3, 0.01);
}

TEST(TestSuite, test_distribution_confidence_interval)
{
    std::default_random_engine generator{};
    std::normal_distribution<double> nd(5.0, 2.0);
    int n_namples = 1000000;
    double p_value = 0.99;

    Distribution distribution{};
    for (int i = 0; i < n_namples; ++i)
    {
        double number = nd(generator);
        distribution.add_sample(number);
    }
    auto conf_interval = distribution.confidence_interval(p_value);

    int n_samples_out_of_bound = 0;
    for (int i = 0; i < n_namples; ++i)
    {
        double number = nd(generator);
        if (number < conf_interval.first || number > conf_interval.second)
        {
            n_samples_out_of_bound++;
        }
    }

    double expected_oob_samples = n_namples * (1 - p_value);
    EXPECT_NEAR(n_samples_out_of_bound, expected_oob_samples, .05 * expected_oob_samples);
}