#include "simulator/time_keeper.hpp"

void Time_keeper::increment(Sim_time dt)
{
    time += dt;
}

void Time_keeper::reset()
{
    time = Sim_time::zero();
}

[[nodiscard]] Sim_time Time_keeper::get_dynamic_time_step(Sim_time mh_dt, Sim_time oh_dt, Sim_time buff_dt,
                                                          Sim_time sim_dt, Sim_time spell_dt, Sim_time slam_dt)
{
    Sim_time dt{Sim_time::from_seconds(100)};
    if (slam_dt.milliseconds() > 0)
    {
        dt = std::min(slam_dt, dt);
    }
    dt = std::min(mh_dt, dt);
    dt = std::min(oh_dt, dt);
    dt = std::min(buff_dt, dt);
    dt = std::min(sim_dt, dt);
    dt = std::min(spell_dt, dt);
    dt += Sim_time::from_milliseconds(1);
    return dt;
}
