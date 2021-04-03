#ifndef WOW_SIMULATOR_STRING_HELPERS_H
#define WOW_SIMULATOR_STRING_HELPERS_H

#include <string>
#include <vector>

namespace String_helpers
{
std::string percent_to_str(double value);

std::string percent_to_str(const std::string& stat_name, double value, const std::string& description);

std::string percent_to_str(const std::string& stat_name, double value1, const std::string& description1, double value2,
                           const std::string& description2);

std::string string_with_precision(int amount);

std::string string_with_precision(double amount, size_t precision);

template <typename T>
bool does_vector_contain(const std::vector<T>& vec, const T& match);

bool find_string(const std::vector<std::string>& string_vec, const std::string& match_string);

double find_value(const std::vector<std::string>& string_vec, const std::vector<double>& double_vec,
                  const std::string& match_string);

} // namespace String_helpers

#include "string_helpers.tcc"

#endif // WOW_SIMULATOR_STRING_HELPERS_H
