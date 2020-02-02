#ifndef WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
#define WOW_SIMULATOR_COMBAT_SIMULATOR_HPP

#include <vector>
#include <cstdlib>
#include <iostream>
#include <cassert>

class Combat_simulator
{
public:
    Combat_simulator() = delete;

    explicit Combat_simulator(std::vector<double> hit_probabilities) : hit_probabilities_{std::move(hit_probabilities)} {}

    double generate_hit(double damage);

private:
    std::vector<double> hit_probabilities_;
    std::vector<double> damage_weights;
};

#endif //WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
