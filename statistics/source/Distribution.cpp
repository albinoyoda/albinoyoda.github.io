#include "Distribution.hpp"

#include "Statistics.hpp"

namespace
{
double expected_samples_to_observe(double z_value)
{
    double p_val = Statistics::normalCDF(z_value);
    return 1.0 / (1.0 - p_val);
}
} // namespace

void Distribution::add_sample(const double sample)
{
    if (n_samples_ == 0)
    {
        max_ = min_ = sample;
    }
    n_samples_++;
    if (sample > max_)
    {
        max_ = sample;
    }
    else if (sample < min_)
    {
        min_ = sample;
    }
    variance_ = Statistics::update_variance(variance_, mean_, n_samples_, sample);
    mean_ = Statistics::update_mean(mean_, n_samples_, sample);
}

void Distribution::reset()
{
    n_samples_ = mean_ = variance_ = min_ = max_ = 0;
}

double Distribution::std_() const
{
    return std::sqrt(variance_);
}

double Distribution::std_of_the_mean() const
{
    return Statistics::sample_deviation(std_(), n_samples_);
}

std::pair<double, double> Distribution::confidence_interval(double p_value) const
{
    double val = Statistics::find_cdf_quantile(Statistics::get_two_sided_p_value(p_value), 0.01);
    return std::pair<double, double>{mean_ - val * std_(), mean_ + val * std_()};
}

std::pair<double, double> Distribution::confidence_interval_of_the_mean(double p_value) const
{
    double val = Statistics::find_cdf_quantile(Statistics::get_two_sided_p_value(p_value), 0.01);
    double std_ = std_of_the_mean();
    return std::pair<double, double>{mean_ - val * std_, mean_ + val * std_};
}

bool Distribution::is_normal_distributed() const
{
    if (n_samples_ < 5)
    {
        return false;
    }
    double st_dev = std_();
    double z_min = (mean_ - min_) / st_dev;
    double n_samples_nominal_min = expected_samples_to_observe(z_min);

    double z_max = (max_ - mean_) / st_dev;
    double n_samples_nominal_max = expected_samples_to_observe(z_max);

    return n_samples_ > std::max(n_samples_nominal_min, n_samples_nominal_max);
}
