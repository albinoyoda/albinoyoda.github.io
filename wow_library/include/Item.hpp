#ifndef WOW_SIMULATOR_ITEM_HPP
#define WOW_SIMULATOR_ITEM_HPP

#include <utility>

#include "Attributes.hpp"

enum class Hand
{
    main_hand,
    off_hand
};

enum class Socket
{
    head,
    neck,
    shoulder,
    back,
    chest,
    wrists,
    hands,
    belt,
    legs,
    boots,
    ring,
    trinket,
    ranged,
    main_hand,
    one_hand,
    off_hand,
};

enum class Set
{
    none,
    devilsaur,
    black_dragonscale,
    rare_pvp_set,
};

class Stat_base
{
public:
    Stat_base() = delete;

    Stat_base(Attributes stats, Special_stats special_stats)
            :
            stats_(stats),
            special_stats_(special_stats) {};

    constexpr const Attributes &get_stats() const
    {
        return stats_;
    }

    constexpr const Special_stats &get_special_stats() const
    {
        return special_stats_;
    }

private:
    Attributes stats_;
    Special_stats special_stats_;

};

class Armor : public Stat_base
{
public:
    Armor() = delete;

    Armor(std::string name, Attributes stats, Special_stats special_stats, Socket socket, Set set_name = Set::none) :
            Stat_base{stats, special_stats},
            name_(std::move(name)),
            socket_(socket),
            set_name_(set_name) {};

    constexpr const std::string &get_name() const
    {
        return name_;
    }

    constexpr const Socket &get_socket() const
    {
        return socket_;
    }

    constexpr const Set &get_set() const
    {
        return set_name_;
    }

private:
    std::string name_;
    Socket socket_;
    Set set_name_;
};

class Weapon : public Armor
{
public:
    Weapon() = delete;

    Weapon(std::string name, double swing_speed, std::pair<double, double> damage_interval, Attributes stats,
           Special_stats special_stats, Socket socket, Skill_type skill_type, Set set_name = Set::none) :
            Armor{std::move(name), stats, special_stats, socket, set_name},
            swing_speed_{swing_speed},
            damage_interval_{std::move(damage_interval)},
            weapon_type_{skill_type} {}

    constexpr const double &get_swing_speed() const
    {
        return swing_speed_;
    }

    constexpr const std::pair<double, double> &get_damage_interval() const
    {
        return damage_interval_;
    }

    constexpr const Skill_type &get_weapon_type() const
    {
        return weapon_type_;
    }

private:
    double swing_speed_;
    std::pair<double, double> damage_interval_;
    Skill_type weapon_type_;
};

class Set_bonus : public Stat_base
{
public:
    Set_bonus() = delete;

    Set_bonus(Attributes stats, Special_stats special_stats, size_t pieces, Set set_name) :
            Stat_base{stats, special_stats},
            pieces_(pieces),
            set_name_(set_name) {};

    constexpr const size_t &get_pieces() const
    {
        return pieces_;
    }

    constexpr const Set &get_set_name() const
    {
        return set_name_;
    }

private:
    size_t pieces_;
    Set set_name_;
};

class Buff : public Stat_base
{
public:
    Buff() = delete;

    Buff(std::string name, Attributes stats, Special_stats special_stats, double stat_multiplier) :
            Stat_base{stats, special_stats},
            name_(std::move(name)), stat_multiplier_(stat_multiplier) {};

    double get_stat_multiplier() const
    {
        return stat_multiplier_;
    }

private:
    std::string name_;
    double stat_multiplier_;
};

class Weapon_buff : public Stat_base
{
public:
    Weapon_buff() = delete;

    Weapon_buff(std::string name, Attributes stats, Special_stats special_stats, double oh_bonus_damage,
                double mh_bonus_damage) :
            Stat_base{stats, special_stats},
            name_(std::move(name)),
            oh_bonus_damage_{oh_bonus_damage},
            mh_bonus_damage_{mh_bonus_damage} {};

    double get_oh_bonus_damage() const
    {
        return oh_bonus_damage_;
    }

    double get_mh_bonus_damage() const
    {
        return mh_bonus_damage_;
    }

private:
    std::string name_;
    double oh_bonus_damage_;
    double mh_bonus_damage_;
};

std::ostream &operator<<(std::ostream &os, Socket const &socket)
{
    os << "Item slot ";
    switch (socket)
    {
        case Socket::head:
            os << "head." << "\n";
            break;
        case Socket::neck:
            os << "neck." << "\n";
            break;
        case Socket::shoulder:
            os << "shoulder." << "\n";
            break;
        case Socket::back:
            os << "back." << "\n";
            break;
        case Socket::chest:
            os << "chest." << "\n";
            break;
        case Socket::wrists:
            os << "wrists." << "\n";
            break;
        case Socket::hands:
            os << "hands." << "\n";
            break;
        case Socket::belt:
            os << "belt." << "\n";
            break;
        case Socket::legs:
            os << "legs." << "\n";
            break;
        case Socket::boots:
            os << "boots." << "\n";
            break;
        case Socket::ring:
            os << "ring." << "\n";
            break;
        case Socket::trinket:
            os << "trinket." << "\n";
            break;
        case Socket::ranged:
            os << "ranged." << "\n";
            break;
        case Socket::main_hand:
            os << "ranged." << "\n";
            break;
        case Socket::off_hand:
            os << "ranged." << "\n";
            break;
        case Socket::one_hand:
            assert(true);
            break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Armor &armor)
{
    os << armor.get_name() << "\n";
    return os;
}

#endif //WOW_SIMULATOR_ITEM_HPP

