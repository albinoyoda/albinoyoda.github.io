#ifndef WOW_SIMULATOR_TIME_KEEPER_HPP
#define WOW_SIMULATOR_TIME_KEEPER_HPP

#include <algorithm>

class Time_keeper
{
public:
    Time_keeper() = default;

    void increment(double dt)
    {
        blood_thirst_cd -= dt;
        whirlwind_cd -= dt;
        global_cd -= dt;
        time += dt;
    }

    void reset()
    {
        blood_thirst_cd = -1e-10;
        whirlwind_cd = -1e-10;
        global_cd = -1e-10;
        time = 0.0;
    }

    constexpr double get_dynamic_time_step(double mh_dt,
                                           double oh_dt,
                                           double buff_dt,
                                           double sim_dt)
    {
        double dt = 100.0;
        if (blood_thirst_cd > 0.0)
        {
            dt = std::min(blood_thirst_cd, dt);
        }
        if (whirlwind_cd > 0.0)
        {
            dt = std::min(whirlwind_cd, dt);
        }
        if (global_cd > 0.0)
        {
            dt = std::min(global_cd, dt);
        }
        dt = std::min(mh_dt, dt);
        dt = std::min(oh_dt, dt);
        dt = std::min(buff_dt, dt);
        dt = std::min(sim_dt, dt);
        dt += 1e-5;
        return dt;
    }

    double blood_thirst_cd;
    double whirlwind_cd;
    double global_cd;
    double time;
};


#endif //WOW_SIMULATOR_TIME_KEEPER_HPP
