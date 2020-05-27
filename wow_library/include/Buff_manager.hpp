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


struct Aura_uptime
{
    Aura_uptime() = default;

    struct Aura
    {
        Aura(std::string id, double duration) :
                id(std::move(id)),
                duration(duration) {};

        std::string id;
        double duration;
    };

    void add(const std::string &name, double duration)
    {
        for (auto &aura : auras)
        {
            if (name == aura.id)
            {
                aura.duration += duration;
                return;
            }
        }
        auras.emplace_back(name, duration);
    }

    std::vector<Aura> auras;
};

class Buff_manager
{
public:
    struct Proc
    {
        Proc(std::string id, int counter) :
                id(std::move(id)),
                counter(counter) {};

        std::string id;
        int counter;
    };

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
            aura_uptime.add(stat_gains[i].id, dt);
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

    void increment_proc(const std::string &name)
    {
        for (auto &proc : procs)
        {
            if (name == proc.id)
            {
                proc.counter++;
                return;
            }
        }
        procs.emplace_back(name, 1);
    }

    Special_stats *simulation_special_stats;
    std::vector<Combat_buff> stat_gains;
    Aura_uptime aura_uptime;
    std::vector<Proc> procs;
};

#endif //WOW_SIMULATOR_BUFF_MANAGER_HPP
