#ifndef WOW_SIMULATOR_SLAM_MANAGER_HPP
#define WOW_SIMULATOR_SLAM_MANAGER_HPP

#include <common/sim_time.hpp>

struct Slam_manager
{
    Slam_manager() = default;
    Slam_manager(Sim_time slam_cast_time) : slam_cast_time_{slam_cast_time} {};

    constexpr void reset()
    {
        slam_queued_ = false;
        slam_queue_time_stamp_ = Sim_time::zero();
    };

    [[nodiscard]] constexpr bool is_slam_queued() const { return slam_queued_; }

    constexpr void queue_slam(Sim_time time_stamp)
    {
        slam_queued_ = true;
        slam_queue_time_stamp_ = time_stamp;
    }

    constexpr void un_queue_slam() { slam_queued_ = false; }

    constexpr Sim_time time_left(Sim_time current_time)
    {
        return slam_queued_ ? slam_cast_time_ - (current_time - slam_queue_time_stamp_) : Sim_time::from_seconds(100);
    }

private:
    Sim_time slam_cast_time_ = Sim_time::zero();
    bool slam_queued_{false};
    Sim_time slam_queue_time_stamp_{};
};

#endif // WOW_SIMULATOR_SLAM_MANAGER_HPP
