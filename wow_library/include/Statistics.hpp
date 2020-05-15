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
}
#endif //WOW_SIMULATOR_STATISTICS_HPP
