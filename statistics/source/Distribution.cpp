#include "Distribution.hpp"

#include "Statistics.hpp"

void Distribution::add_sample(const double sample)
{
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
    return false;
}
