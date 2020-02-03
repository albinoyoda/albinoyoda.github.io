#ifndef WOW_SIMULATOR_ENCHANT_HPP
#define WOW_SIMULATOR_ENCHANT_HPP


#include "Stats.hpp"

class Enchant
{
public:
    enum class Socket
    {
        head,
        back,
        chest,
        wrists,
        hands,
        legs,
    };

    enum class Type
    {
        strength,
        strength7,
        strength9,
        agility,
        agility7,
        agility9,
        haste,
        crusader,
        minor_stats,
        major_stats
    };

    Enchant() = delete;

    Enchant(Socket socket, Type type);

    Special_stats convert_to_special_stats() const;

    double get_haste() const;

private:
    Stats stats_;
public:
    const Stats &get_stats() const;

private:
    double haste;
    Socket socket_;
    Type type_;
};

#endif //WOW_SIMULATOR_ENCHANT_HPP
