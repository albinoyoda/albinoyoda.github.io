#include <cassert>
#include "Enchant.hpp"

Enchant::Enchant(Enchant::Socket socket, Enchant::Type type)
        : stats_{},
          haste{1.0},
          crusader_mh_{false},
          crusader_oh_{false},
          socket_{socket},
          type_{type}
{
    switch (socket)
    {
        case Enchant::Socket::head:
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    stats_ = Stats{0, 8};

                    break;
                case Enchant::Type::strength:
                    stats_ = Stats{8, 0};
                    break;
                case Enchant::Type::haste:
                    haste = 1.01;
                    break;
                default:
                    assert(false);
            }
            break;
        }
        case Enchant::Socket::back:
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    stats_ = Stats{0, 3};
                    break;
                default:
                    assert(false);
            }
            break;
        }
        case Enchant::Socket::chest:
        {
            switch (type)
            {
                case Enchant::Type::minor_stats:
                    stats_ = Stats{3, 3};
                    break;
                case Enchant::Type::major_stats:
                    stats_ = Stats{4, 4};
                    break;
                default:
                    assert(false);
            }
            break;
        }
        case Enchant::Socket::wrists :
        {
            switch (type)
            {
                case Enchant::Type::agility7:
                    stats_ = Stats{0, 7};
                    break;
                case Enchant::Type::agility9:
                    stats_ = Stats{0, 9};
                    break;
                case Enchant::Type::strength7:
                    stats_ = Stats{7, 0};
                    break;
                case Enchant::Type::strength9:
                    stats_ = Stats{9, 0};
                    break;
                default:
                    assert(false);
            }
            break;
        }
        case Enchant::Socket::hands :
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    stats_ = Stats{0, 7};
                    break;
                case Enchant::Type::strength:
                    stats_ = Stats{7, 0};
                    break;
                case Enchant::Type::haste:
                    haste = 1.01;
                    break;
                default:
                    assert(false);
            }
            break;
        }
        case Enchant::Socket::legs :
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    stats_ = Stats{0, 8};
                    break;
                case Enchant::Type::strength:
                    stats_ = Stats{8, 0};
                    break;
                case Enchant::Type::haste:
                    haste = 1.01;
                    break;
                default:
                    assert(false);
            }
            break;
        }
        case Enchant::Socket::boots :
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    stats_ = Stats{0, 7};
                    break;
                default:
                    assert(false);
            }
            break;
        }
        case Enchant::Socket::weapon_mh:
        {
            switch (type)
            {
                case Enchant::Type::crusader:
                    crusader_mh_ = true;
                    break;
                default:
                    assert(false);
            }
            break;
        }
        case Enchant::Socket::weapon_oh:
        {
            switch (type)
            {
                case Enchant::Type::crusader:
                    crusader_oh_ = true;
                    break;
                default:
                    assert(false);
            }
            break;
        }
    }
}

Special_stats Enchant::convert_to_special_stats() const
{
    return {stats_.convert_to_special_stats()};
}

double Enchant::get_haste() const
{
    return haste;
}

const Stats &Enchant::get_stats() const
{
    return stats_;
}

bool Enchant::is_crusader_mh() const
{
    return crusader_mh_;
}

bool Enchant::is_crusader_oh() const
{
    return crusader_oh_;
}
