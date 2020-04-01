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
        crusader_mh_buff_timer -= dt;
        crusader_oh_buff_timer -= dt;
        time += dt;
        step_index++;
    }

    void reset()
    {
        blood_thirst_cd = -1e-10;
        whirlwind_cd = -1e-10;
        global_cd = -1e-10;
        crusader_mh_buff_timer = -1e-10;
        crusader_oh_buff_timer = -1e-10;
        time = 0.0;
        step_index = 1;
    }

    constexpr double get_dynamic_time_step(double mh_dt,
                                           double oh_dt,
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
        if (crusader_mh_buff_timer > 0.0)
        {
            dt = std::min(crusader_mh_buff_timer, dt);
        }
        if (crusader_oh_buff_timer > 0.0)
        {
            dt = std::min(crusader_oh_buff_timer, dt);
        }
        dt = std::min(mh_dt, dt);
        dt = std::min(oh_dt, dt);
        dt = std::min(sim_dt, dt);
        dt += 1e-5;
        current_dt = dt;
        return dt;
    }

    double blood_thirst_cd;
    double whirlwind_cd;
    double global_cd;
    double crusader_mh_buff_timer;
    double crusader_oh_buff_timer;
    double time;
    double current_dt;
    int step_index;
};


#endif //WOW_SIMULATOR_TIME_KEEPER_HPP
