#include "string_helpers.hpp"

#include <iomanip>
#include <iostream>
#include <sstream> // For some reason this is required by emscripten

namespace String_helpers
{
std::string percent_to_str(double value)
{
    std::ostringstream stream;
    stream << std::setprecision(3) << value << "%";
    return stream.str();
}

std::string percent_to_str(const std::string& stat_name, double value, const std::string& description)
{
    std::ostringstream stream;
    stream << stat_name << ": " << std::setprecision(3) << "<b>" << value << "%</b> " << description << "<br>";
    return stream.str();
}

std::string percent_to_str(const std::string& stat_name, double value1, const std::string& description1, double value2,
                           const std::string& description2)
{
    std::ostringstream stream;
    stream << stat_name << ": " << std::setprecision(3) << "<b>" << value1 << "%</b> " << description1 << ". (<b>"
           << value2 << "%</b> " << description2 << ")<br>";
    return stream.str();
}

std::string string_with_precision(int amount)
{
    std::ostringstream stream;
    stream << amount;
    return stream.str();
}

std::string string_with_precision(double amount, size_t precision)
{
    std::ostringstream stream;
    stream << std::setprecision(precision) << amount;
    return stream.str();
}

bool find_string(const std::vector<std::string>& string_vec, const std::string& match_string)
{
    for (const auto& string : string_vec)
    {
        if (string == match_string)
        {
            return true;
        }
    }
    return false;
}

double find_value(const std::vector<std::string>& string_vec, const std::vector<double>& double_vec,
                  const std::string& match_string)
{
    size_t index = 0;
    for (const auto& string : string_vec)
    {
        if (string == match_string)
        {
            return double_vec[index];
        }
        index++;
    }
    std::cout << "WARN: Could not find: " << match_string << std::endl;
    return 0.0;
}
} // namespace String_helpers