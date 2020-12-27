#include <algorithm>

template <typename T>
bool does_vector_contain(const std::vector<T>& vec, const T& match)
{
    for (const auto& elem : vec)
    {
        if (elem == match)
        {
            return true;
        }
    }
    return false;
}

template <typename T>
Find_values<T>::Find_values(const std::vector<std::string>& string_vec, const std::vector<T>& value_vec)
    : string_vec(string_vec), value_vec(value_vec)
{
    if (string_vec.size() != value_vec.size())
    {
        std::cout << "Cant create find values class with two vectors of different size!";
        assert(string_vec.size() == value_vec.size());
    }
}

template <typename T>
T Find_values<T>::find(const std::string& name)
{
    auto it = std::find(string_vec.cbegin(), string_vec.cend(), name);
    if (it != string_vec.cend())
    {
        auto pos = it - string_vec.cbegin();
        return value_vec[pos];
    }
    std::cout << "ERROR: Could not find: " << name << std::endl;
    return {0};
}
