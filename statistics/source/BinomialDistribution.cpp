#include "BinomialDistribution.hpp"

#include "Statistics.hpp"

BinomialDistribution::BinomialDistribution(double trials, double success_rate)
{
    mean_ = trials * success_rate;
    variance_ = mean_ * (1 - success_rate);
}

double BinomialDistribution::std_() const
{
    return std::sqrt(variance_);
}

std::pair<double, double> BinomialDistribution::confidence_interval(double p_value) const
{
    // Assuming its symmetric, which generally is only true when success_rate is close to 0.5
    double val = Statistics::find_cdf_quantile(Statistics::get_two_sided_p_value(p_value), 0.01);
    return std::pair<double, double>{mean_ - val * std_(), mean_ + val * std_()};
}

double BinomialDistribution::confidence_interval_width(double p_value) const
{
    auto conf_interval = confidence_interval(p_value);
    return conf_interval.second - conf_interval.first;
}
