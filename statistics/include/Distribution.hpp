#ifndef WOW_SIMULATOR_DISTRIBUTION_HPP
#define WOW_SIMULATOR_DISTRIBUTION_HPP

#include <cstdint>
#include <utility>

class Distribution
{
public:
    Distribution() = default;

    void add_sample(double sample);

    [[nodiscard]] double std_() const;

    [[nodiscard]] double std_of_the_mean() const;

    [[nodiscard]] bool is_normal_distributed() const;

    [[nodiscard]] std::pair<double, double> confidence_interval(double quantile) const;

    [[nodiscard]] std::pair<double, double> confidence_interval_of_the_mean(double quantile) const;

    double mean_;
    double variance_;
    double max_;
    double min_;
    int64_t n_samples_;
};

#endif // WOW_SIMULATOR_DISTRIBUTION_HPP
