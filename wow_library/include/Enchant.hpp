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
        boots,
        weapon_mh,
        weapon_oh,
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

    const Stats &get_stats() const;

    bool is_crusader_mh() const;

    bool is_crusader_oh() const;

private:
    Stats stats_;
    double haste;
    bool crusader_mh_;
    bool crusader_oh_;
    Socket socket_;
    Type type_;
};

#endif //WOW_SIMULATOR_ENCHANT_HPP

