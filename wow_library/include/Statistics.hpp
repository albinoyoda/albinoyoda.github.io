#ifndef WOW_SIMULATOR_STATISTICS_HPP
#define WOW_SIMULATOR_STATISTICS_HPP

#include <vector>
#include <cmath>

namespace Statistics
{
    double average(const std::vector<double> &vec)
    {
        double sum = 0;
        for (double value : vec)
        {
            sum += value;
        }
        return sum / vec.size();
    }

    double variance(const std::vector<double> &vec, double average)
    {
        double sum = 0;
        double inverse = 1.0 / static_cast<double>(vec.size());
        for (double value : vec)
        {
            sum += std::pow(static_cast<double>(value) - average, 2);
        }
        return inverse * sum;
    }

    double standard_deviation(const std::vector<double> &vec, double average)
    {
        return std::sqrt(variance(vec, average));
    }

    double sample_deviation(double mean, int n_samples)
    {
        return mean / std::sqrt(static_cast<double>(n_samples));
    }

    double add_standard_deviations(double std1, double std2)
    {
        return std::sqrt(std1 * std1 + std2 * std2);
    }
}
#endif //WOW_SIMULATOR_STATISTICS_HPP
