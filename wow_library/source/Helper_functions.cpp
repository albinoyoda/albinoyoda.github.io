#include "Helper_functions.hpp"

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

Race get_race(const std::string& race)
{
    if (race == "human")
    {
        return Race::human;
    }
    else if (race == "gnome")
    {
        return Race::gnome;
    }
    else if (race == "dwarf")
    {
        return Race::dwarf;
    }
    else if (race == "night_elf")
    {
        return Race::night_elf;
    }
    else if (race == "orc")
    {
        return Race::orc;
    }
    else if (race == "troll")
    {
        return Race::troll;
    }
    else if (race == "undead")
    {
        return Race::undead;
    }
    else if (race == "tauren")
    {
        return Race::tauren;
    }
    else
    {
        std::cout << "Race not found!!! picking human"
                  << "\n";
        return Race::human;
    }
}

Character get_character_of_race(const std::string& race)
{
    if (race == "human")
    {
        return {Race::human, 60};
    }
    else if (race == "gnome")
    {
        return {Race::gnome, 60};
    }
    else if (race == "dwarf")
    {
        return {Race::dwarf, 60};
    }
    else if (race == "night_elf")
    {
        return {Race::night_elf, 60};
    }
    else if (race == "orc")
    {
        return {Race::orc, 60};
    }
    else if (race == "troll")
    {
        return {Race::troll, 60};
    }
    else if (race == "undead")
    {
        return {Race::undead, 60};
    }
    else if (race == "tauren")
    {
        return {Race::tauren, 60};
    }
    else
    {
        std::cout << "Race not found!!! picking human"
                  << "\n";
        return {Race::human, 60};
    }
}

std::string string_with_precision(double amount, size_t precision)
{
    std::ostringstream stream;
    stream << std::setprecision(precision) << amount;
    return stream.str();
}