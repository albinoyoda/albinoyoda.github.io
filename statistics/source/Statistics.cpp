#include "Statistics.hpp"

#include <cmath>

namespace Statistics
{
double average(const std::vector<double>& vec)
{
    double sum = 0;
    for (double value : vec)
    {
        sum += value;
    }
    return sum / vec.size();
}

double variance(const std::vector<double>& vec, double average)
{
    double sum = 0;
    double inverse = 1.0 / static_cast<double>(vec.size());
    for (double value : vec)
    {
        sum += std::pow(static_cast<double>(value) - average, 2);
    }
    return inverse * sum;
}

double standard_deviation(const std::vector<double>& vec, double average)
{
    // TODO this is always the population standard deviation
    return std::sqrt(variance(vec, average));
}

double sample_deviation(double standard_dev, int n_samples)
{
    return standard_dev / std::sqrt(static_cast<double>(n_samples));
}

double add_standard_deviations(double std1, double std2)
{
    return std::sqrt(std1 * std1 + std2 * std2);
}

double update_mean(double mean, int tot_samples, double new_sample)
{
    return (mean * (tot_samples - 1) + new_sample) / tot_samples;
}

double update_variance(double variance, double mean, int tot_samples, double new_sample)
{
    double new_mean = (mean * (tot_samples - 1) + new_sample) / tot_samples;
    return ((tot_samples - 1) * variance + (new_sample - new_mean) * (new_sample - mean)) / tot_samples;
}

double get_two_sided_p_value(double p_value)
{
    return 1 - ((1 - p_value) / 2.0);
}

double normalCDF(double value)
{
    return 0.5 * erfc(-value * M_SQRT1_2);
}

double find_cdf_quantile(double target_quantile, double precision)
{
    double x = 0.0;
    double quantile = 0.5;
    while (quantile < target_quantile)
    {
        x += precision;
        quantile = normalCDF(x);
    }
    return x;
}

} // namespace Statistics