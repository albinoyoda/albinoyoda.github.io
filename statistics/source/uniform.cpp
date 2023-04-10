#include "statistics/uniform.hpp"

#include <cstdlib>

double get_uniform_random(double r_max)
{
    return rand() * r_max / RAND_MAX;
}
