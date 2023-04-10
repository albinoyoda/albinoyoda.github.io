#ifndef WOW_SIMULATOR_TIME_KEEPER_HPP
#define WOW_SIMULATOR_TIME_KEEPER_HPP

#include "common/sim_time.hpp"
#include "damage_sources.hpp"

class Time_keeper
{
public:
    void increment(Sim_time dt);

    void reset();

    [[nodiscard]] static Sim_time get_dynamic_time_step(Sim_time mh_dt, Sim_time oh_dt, Sim_time buff_dt,
                                                        Sim_time sim_dt, Sim_time spell_dt, Sim_time slam_dt);

    Sim_time time{};
};

#endif // WOW_SIMULATOR_TIME_KEEPER_HPP
