#ifndef WOW_SIMULATOR_FIND_VALUES_H
#define WOW_SIMULATOR_FIND_VALUES_H

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cassert>


template <typename T>
class Find_values
{
public:
    Find_values(const std::vector<std::string>& string_vec, const std::vector<T>& value_vec);

    T find(const std::string& name);

private:
    const std::vector<std::string>& string_vec;
    const std::vector<T>& value_vec;
};

#include "find_values.tcc"

#endif // WOW_SIMULATOR_FIND_VALUES_H
