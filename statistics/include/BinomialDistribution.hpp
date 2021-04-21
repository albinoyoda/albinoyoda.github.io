#ifndef WOW_SIMULATOR_BINOMIALDISTRIBUTION_HPP
#define WOW_SIMULATOR_BINOMIALDISTRIBUTION_HPP

#include <utility>

class BinomialDistribution
{
public:
    BinomialDistribution(double trials, double success_rate);

    [[nodiscard]] double std_() const;

    [[nodiscard]] std::pair<double, double> confidence_interval(double quantile) const;

    [[nodiscard]] double confidence_interval_width(double quantile) const;

    double mean_;
    double variance_;
};

#endif // WOW_SIMULATOR_BINOMIALDISTRIBUTION_HPP
