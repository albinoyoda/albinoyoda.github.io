#ifndef WOW_SIMULATOR_STATISTICS_HPP
#define WOW_SIMULATOR_STATISTICS_HPP

#include <vector>

namespace Statistics
{
    double average(const std::vector<double> &vec);

    double variance(const std::vector<double> &vec, double average);

    double standard_deviation(const std::vector<double> &vec, double average);

    double sample_deviation(double standard_dev, int n_samples);

    double add_standard_deviations(double std1, double std2);

    constexpr double update_mean(double mean, int tot_samples, double new_sample)
    {
        return (mean * (tot_samples - 1) + new_sample) / tot_samples;
    }

    constexpr double update_variance(double variance, double mean, int tot_samples, double new_sample)
    {
        double new_mean = (mean * (tot_samples - 1) + new_sample) / tot_samples;
        return ((tot_samples - 1) * variance + (new_sample - new_mean) * (new_sample - mean)) / tot_samples;
    }
}
#endif //WOW_SIMULATOR_STATISTICS_HPP
