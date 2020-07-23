#ifndef WOW_SIMULATOR_BUFF_MANAGER_HPP
#define WOW_SIMULATOR_BUFF_MANAGER_HPP

#include "Attributes.hpp"
#include "Item.hpp"

#include <cmath>
#include <unordered_map>
#include <vector>

struct Over_time_buff
{
    Over_time_buff(std::string id, Special_stats special_stats, int init_time, double rage_gain, double damage,
                   int interval, int duration)
        : id(std::move(id))
        , special_stats(special_stats)
        , init_time(init_time)
        , total_ticks(duration / interval)
        , current_ticks(1)
        , rage_gain(rage_gain)
        , damage(damage)
        , interval(interval){};

    std::string id;
    Special_stats special_stats;
    int init_time;
    int total_ticks;
    int current_ticks;
    double rage_gain;
    double damage;
    int interval;
};

struct Combat_buff
{
    Combat_buff(std::string id, Special_stats special_stats, double duration_left)
        : id(std::move(id)), special_stats(special_stats), duration_left(duration_left){};

    std::string id;
    Special_stats special_stats;
    double duration_left;
};

struct Hit_buff
{
    Hit_buff(std::string id, double duration_left) : id(std::move(id)), duration_left(duration_left){};

    std::string id;
    double duration_left;
};

struct Aura_uptime
{
    Aura_uptime() = default;

    struct Aura
    {
        Aura(std::string id, double duration) : id(std::move(id)), duration(duration){};

        std::string id;
        double duration;
    };

    void add(const std::string& name, double duration)
    {
        for (auto& aura : auras)
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
        Proc(std::string id, int counter) : id(std::move(id)), counter(counter){};

        std::string id;
        int counter;
    };

    void initialize(Special_stats& special_stats, const std::vector<Use_effect>& use_effects_input,
                    std::vector<Hit_effect>& hit_effects_mh_input, std::vector<Hit_effect>& hit_effects_oh_input,
                    bool performance_mode_in)
    {
        performance_mode = performance_mode_in;
        stat_gains.clear();
        hit_gains.clear();
        over_time_buffs.clear();
        simulation_special_stats = &special_stats;
        hit_effects_mh = &hit_effects_mh_input;
        hit_effects_oh = &hit_effects_oh_input;
        use_effects = use_effects_input;
    };

    double get_dt(double sim_time)
    {
        double dt = 1e10;
        dt = std::min(dt, next_event);
        double interval_dt = double(min_interval) - std::fmod(sim_time, double(min_interval));
        dt = std::min(dt, interval_dt);
        return dt;
    }

    void increment(double dt, double current_time, double time_left, double& rage, double& global_cooldown,
                   std::vector<std::string>& status, bool debug)
    {
        size_t i = 0;
        next_event = 10;
        while (i < stat_gains.size())
        {
            if (!performance_mode)
            {
                aura_uptime.add(stat_gains[i].id, dt);
            }
            stat_gains[i].duration_left -= dt;
            if (stat_gains[i].duration_left < 0.0)
            {
                if (debug)
                {
                    status.emplace_back(stat_gains[i].id + " fades.");
                }
                if (stat_gains[i].special_stats.hit > 0.0 || stat_gains[i].special_stats.critical_strike > 0.0)
                {
                    need_to_recompute_hittables = true;
                }
                (*simulation_special_stats) -= stat_gains[i].special_stats;
                stat_gains.erase(stat_gains.begin() + i);
            }
            else
            {
                next_event = std::min(next_event, stat_gains[i].duration_left);
                ++i;
            }
        }
        i = 0;
        while (i < hit_gains.size())
        {
            if (!performance_mode)
            {
                aura_uptime.add(hit_gains[i].id, dt);
            }
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
                next_event = std::min(next_event, hit_gains[i].duration_left);
                ++i;
            }
        }
        i = 0;
        while (i < use_effects.size())
        {
            if (time_left - use_effects[i].duration - 1.5 < 0.0 && global_cooldown < 0.0 &&
                rage >= -use_effects[i].rage_boost)
            {
                if (debug)
                {
                    status.emplace_back("Activating: " + use_effects[i].name);
                }
                if (!use_effects[i].hit_effects.empty())
                {
                    add_hit_effect(use_effects[i].name, use_effects[i].hit_effects[0],
                                   use_effects[i].hit_effects[0].duration);
                }
                else if (!use_effects[i].over_time_effects.empty())
                {
                    add_over_time_effect(use_effects[i].over_time_effects[0], int(current_time + 1));
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
        i = 0;
        while (i < over_time_buffs.size())
        {
            if ((int(current_time) - over_time_buffs[i].init_time) / over_time_buffs[i].interval >=
                over_time_buffs[i].current_ticks)
            {
                rage += over_time_buffs[i].rage_gain;
                (*simulation_special_stats) += over_time_buffs[i].special_stats;
                rage = std::min(100.0, rage);
                over_time_buffs[i].current_ticks++;
                if (debug)
                {
                    if (over_time_buffs[i].rage_gain > 0)
                    {
                        status.emplace_back("Over time effect: " + over_time_buffs[i].id +
                                            " tick. Current rage: " + std::to_string(int(rage)));
                    }
                    else if (over_time_buffs[i].damage > 0)
                    {
                        status.emplace_back("Over time effect: " + over_time_buffs[i].id +
                                            " tick. Damage: " + std::to_string(int(over_time_buffs[i].damage)));
                    }
                    else
                    {
                        status.emplace_back("Over time effect: " + over_time_buffs[i].id + " tick.");
                    }
                }
            }
            if (over_time_buffs[i].current_ticks == over_time_buffs[i].total_ticks)
            {
                if (debug)
                {
                    status.emplace_back("Over time effect: " + over_time_buffs[i].id + " fades.");
                }
                over_time_buffs.erase(over_time_buffs.begin() + i);
                if (over_time_buffs.empty())
                {
                    min_interval = 10;
                }
                else
                {
                    for (const auto& over_time_buff : over_time_buffs)
                    {
                        min_interval = std::min(over_time_buff.interval, min_interval);
                    }
                }
            }
            else
            {
                i++;
            }
        }
    }

    void add(const std::string& name, const Special_stats& special_stats, double duration_left)
    {
        for (auto& gain : stat_gains)
        {
            if (name == gain.id)
            {
                gain.duration_left = duration_left;
                return;
            }
        }
        (*simulation_special_stats) += special_stats;
        if (special_stats.hit > 0.0 || special_stats.critical_strike > 0.0)
        {
            need_to_recompute_hittables = true;
        }
        stat_gains.emplace_back(name, special_stats, duration_left);
    }

    void add_hit_effect(const std::string& name, const Hit_effect& hit_effect, double duration_left)
    {
        (*hit_effects_mh).emplace_back(hit_effect);
        (*hit_effects_oh).emplace_back(hit_effect);
        hit_gains.emplace_back(name, duration_left);
    }

    void add_over_time_effect(const Over_time_effect& over_time_effect, int init_time)
    {
        if (over_time_effect.name == "Deep_wounds")
        {
            for (auto& over_time_buff : over_time_buffs)
            {
                if (over_time_buff.id == "Deep_wounds")
                {
                    over_time_buff.damage = std::max(over_time_effect.damage, over_time_buff.damage);
                    over_time_buff.total_ticks =
                        (over_time_effect.duration + init_time - over_time_buff.init_time) / over_time_buff.interval;
                    return;
                }
            }
        }
        min_interval = std::min(over_time_effect.interval, min_interval);
        over_time_buffs.emplace_back(over_time_effect.name, over_time_effect.special_stats, init_time,
                                     over_time_effect.rage_gain, over_time_effect.damage, over_time_effect.interval,
                                     over_time_effect.duration);
    }

    void increment_proc(const std::string& name)
    {
        for (auto& proc : procs)
        {
            if (name == proc.id)
            {
                proc.counter++;
                return;
            }
        }
        procs.emplace_back(name, 1);
    }

    bool need_to_recompute_hittables{false};
    bool performance_mode{false};
    Special_stats* simulation_special_stats;
    std::vector<Combat_buff> stat_gains;
    std::vector<Hit_buff> hit_gains;
    std::vector<Over_time_buff> over_time_buffs;
    std::vector<Hit_effect>* hit_effects_mh;
    std::vector<Hit_effect>* hit_effects_oh;
    std::vector<Use_effect> use_effects;
    double next_event = 10;
    int min_interval = 10;
    Aura_uptime aura_uptime;
    std::vector<Proc> procs;
};

#endif // WOW_SIMULATOR_BUFF_MANAGER_HPP
