#ifndef WOW_SIMULATOR_MATH_HPP
#define WOW_SIMULATOR_MATH_HPP

constexpr double multiplicative_addition(double val1, double val2)
{
    return (1 + val1) * (1 + val2) - 1;
}

constexpr double multiplicative_subtraction(double val1, double val2)
{
    return (1 + val1) / (1 + val2) - 1;
}

#endif // WOW_SIMULATOR_MATH_HPP
