#ifndef WOW_SIMULATOR_STATISTICS_HPP
#define WOW_SIMULATOR_STATISTICS_HPP

#include <cmath>
#include <vector>

namespace Statistics
{
double average(const std::vector<double>& vec);

double variance(const std::vector<double>& vec, double average);

double standard_deviation(const std::vector<double>& vec, double average);

double sample_deviation(double standard_dev, int n_samples);

double add_standard_deviations(double std1, double std2);

double update_mean(double mean, int tot_samples, double new_sample);

double update_variance(double variance, double mean, int tot_samples, double new_sample);

double get_two_sided_p_value(double p_value);

double normalCDF(double value);

double find_cdf_quantile(double target_quantile, double precision);
} // namespace Statistics

#endif // WOW_SIMULATOR_STATISTICS_HPP
