#ifndef WOW_SIMULATOR_BUFF_MANAGER_HPP
#define WOW_SIMULATOR_BUFF_MANAGER_HPP

#include <vector>
#include "Attributes.hpp"

struct Combat_buff
{
    Combat_buff(std::string id, Special_stats special_stats, double duration_left) :
            id(std::move(id)),
            special_stats(special_stats),
            duration_left(duration_left) {};

    std::string id;
    Special_stats special_stats;
    double duration_left;
};

class Buff_manager
{
public:
    void set_target(Special_stats &special_stats)
    {
        stat_gains.clear();
        simulation_special_stats = &special_stats;
    };

    double get_dt()
    {
        double dt = 1e10;
        for (const auto &gain : stat_gains)
        {
            dt = std::min(dt, gain.duration_left);
        }
        return dt;
    }

    void increment(double dt)
    {
        size_t i = 0;
        while (i < stat_gains.size())
        {
            stat_gains[i].duration_left -= dt;
            if (stat_gains[i].duration_left < 0.0)
            {
                (*simulation_special_stats) -= stat_gains[i].special_stats;
                stat_gains.erase(stat_gains.begin() + i);
            }
            else
            {
                ++i;
            }
        }
    }

    void add(const std::string &name, const Special_stats &special_stats, double duration_left)
    {
        for (auto &gain : stat_gains)
        {
            if (name == gain.id)
            {
                gain.duration_left = duration_left;
                return;
            }
        }
        (*simulation_special_stats) += special_stats;
        stat_gains.emplace_back(name, special_stats, duration_left);
    }

    Special_stats *simulation_special_stats;
    std::vector<Combat_buff> stat_gains;
};

#endif //WOW_SIMULATOR_BUFF_MANAGER_HPP
