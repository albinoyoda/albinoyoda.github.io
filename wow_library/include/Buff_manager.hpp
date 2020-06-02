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

struct Hit_buff
{
    Hit_buff(std::string id, double duration_left) :
            id(std::move(id)),
            duration_left(duration_left) {};

    std::string id;
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

    void set_target(Special_stats &special_stats, const std::vector<Use_effect> &use_effects_input,
                    std::vector<Hit_effect> &hit_effects_mh_input, std::vector<Hit_effect> &hit_effects_oh_input)
    {
        stat_gains.clear();
        simulation_special_stats = &special_stats;
        hit_effects_mh = &hit_effects_mh_input;
        hit_effects_oh = &hit_effects_oh_input;
        use_effects = use_effects_input;
    };

    double get_dt()
    {
        double dt = 1e10;
        for (const auto &gain : stat_gains)
        {
            dt = std::min(dt, gain.duration_left);
        }
        for (const auto &gain : hit_gains)
        {
            dt = std::min(dt, gain.duration_left);
        }
        return dt;
    }

    void increment(double dt, double time_left, double &rage, double &global_cooldown, std::vector<std::string> &status,
                   bool debug)
    {
        size_t i = 0;
        while (i < stat_gains.size())
        {
            aura_uptime.add(stat_gains[i].id, dt);
            stat_gains[i].duration_left -= dt;
            if (stat_gains[i].duration_left < 0.0)
            {
                if (debug)
                {
                    status.emplace_back(stat_gains[i].id + " fades.");
                }
                (*simulation_special_stats) -= stat_gains[i].special_stats;
                stat_gains.erase(stat_gains.begin() + i);
            }
            else
            {
                ++i;
            }
        }
        i = 0;
        while (i < hit_gains.size())
        {
            aura_uptime.add(hit_gains[i].id, dt);
            hit_gains[i].duration_left -= dt;
            if (hit_gains[i].duration_left < 0.0)
            {
                if (debug)
                {
                    status.emplace_back(hit_gains[i].id + " fades.");
                }
                for (size_t j = 0; j < (*hit_effects_mh).size(); j++)
                {
                    if (hit_gains[i].id == (*hit_effects_mh)[j].name)
                    {
                        (*hit_effects_mh).erase((*hit_effects_mh).begin() + j);
                    }
                }
                for (size_t j = 0; j < (*hit_effects_oh).size(); j++)
                {
                    if (hit_gains[i].id == (*hit_effects_oh)[j].name)
                    {
                        (*hit_effects_oh).erase((*hit_effects_oh).begin() + i);
                    }
                }
                hit_gains.erase(hit_gains.begin() + i);
            }
            else
            {
                ++i;
            }
        }
        i = 0;
        while (i < use_effects.size())
        {
            if (time_left - use_effects[i].duration - 1.5 < 0.0 && global_cooldown < 0.0)
            {
                if (debug)
                {
                    status.emplace_back("Activating: " + use_effects[i].name);
                }
                if (use_effects[i].hit_effect.name != "")
                {
                    add_hit_effect(use_effects[i].name, use_effects[i].hit_effect, 30);
                }
                else
                {
                    add(use_effects[i].name, use_effects[i].get_special_stat_equivalent(*simulation_special_stats),
                        use_effects[i].duration);
                }
                rage += use_effects[i].rage_boost;
                rage = std::min(100.0, rage);
                use_effects.erase(use_effects.begin() + i);
                if (use_effects[i].triggers_gcd)
                {
                    global_cooldown = 1.5;
                }
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

    void add_hit_effect(const std::string &name, const Hit_effect &hit_effect, double duration_left)
    {
        (*hit_effects_mh).emplace_back(hit_effect);
        (*hit_effects_oh).emplace_back(hit_effect);
        hit_gains.emplace_back(name, duration_left);
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
    std::vector<Hit_effect> *hit_effects_mh;
    std::vector<Hit_effect> *hit_effects_oh;
    std::vector<Combat_buff> stat_gains;
    std::vector<Hit_buff> hit_gains;
    std::vector<Use_effect> use_effects;
    Aura_uptime aura_uptime;
    std::vector<Proc> procs;
};

#endif //WOW_SIMULATOR_BUFF_MANAGER_HPP
