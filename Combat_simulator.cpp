#include "Combat_simulator.hpp"

double Combat_simulator::generate_hit(double damage)
{
    double random_var = 100 * rand() / static_cast<double>(RAND_MAX);

    int outcome = std::lower_bound(hit_probabilities_.begin(), hit_probabilities_.end(), random_var) -
                  hit_probabilities_.begin();

    switch (outcome)
    {
        case 0:
            std::cout << "miss" << "\n";
            return 0.0;
        case 1:
            std::cout << "dodge" << "\n";
            return 0.0;
        case 2:
            std::cout << "glancing" << "\n";
            return damage * 0.85;
        case 3:
            std::cout << "crit" << "\n";
            return damage * 2.2;
        case 4:
            std::cout << "hit" << "\n";
            return damage;
        default:
            std::cout << "BUGGGGGGG" << "\n";
    }
    assert(false);
    return 0.0;
}