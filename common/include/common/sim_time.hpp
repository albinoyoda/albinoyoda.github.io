#ifndef WOW_SIMULATOR_SIM_TIME_HPP
#define WOW_SIMULATOR_SIM_TIME_HPP

#include <cmath>

class Sim_time
{
public:
    constexpr explicit Sim_time(int milli_seconds) noexcept : time_ms_{milli_seconds} {}

    constexpr explicit Sim_time() noexcept : time_ms_{0} {}

    static constexpr Sim_time zero() { return Sim_time{0}; }

    static constexpr Sim_time from_seconds(int seconds) { return Sim_time{seconds * 1000}; }

    static constexpr Sim_time from_seconds(double seconds) { return Sim_time{static_cast<int>(seconds * 1000)}; }

    static constexpr Sim_time from_milliseconds(int milli_seconds) { return Sim_time{milli_seconds}; }

    [[nodiscard]] constexpr int milliseconds() const noexcept { return time_ms_; }

    [[nodiscard]] constexpr double seconds() const noexcept { return time_ms_ / 1000.0; }

    [[nodiscard]] constexpr Sim_time abs() const { return Sim_time{std::abs(time_ms_)}; }

    constexpr Sim_time operator+=(const Sim_time& rhs) noexcept
    {
        time_ms_ += rhs.milliseconds();
        return *this;
    }

    constexpr Sim_time operator-=(const Sim_time& rhs) noexcept
    {
        time_ms_ -= rhs.milliseconds();
        return *this;
    }

private:
    int time_ms_;
};

constexpr Sim_time operator+(const Sim_time& t1, const Sim_time& t2)
{
    return Sim_time{t1.milliseconds() + t2.milliseconds()};
}

constexpr Sim_time operator-(const Sim_time& t1, const Sim_time& t2)
{
    return Sim_time{t1.milliseconds() - t2.milliseconds()};
}

constexpr int operator/(const Sim_time& t1, const Sim_time& t2)
{
    return t1.milliseconds() / t2.milliseconds();
}

constexpr Sim_time operator/(const Sim_time& t1, const double& val)
{
    return Sim_time::from_milliseconds(static_cast<int>(t1.milliseconds() / val));
}

constexpr Sim_time operator*(const Sim_time& t1, const int& val)
{
    return Sim_time::from_milliseconds(t1.milliseconds() * val);
}

constexpr Sim_time operator*(const Sim_time& t1, const double& val)
{
    return Sim_time::from_milliseconds(static_cast<int>(t1.milliseconds() * val));
}

constexpr Sim_time operator*(const int& val, const Sim_time& t1)
{
    return Sim_time::from_milliseconds(t1.milliseconds() * val);
}

constexpr Sim_time operator*(const double& val, const Sim_time& t1)
{
    return Sim_time::from_milliseconds(static_cast<int>(t1.milliseconds() * val));
}

constexpr bool operator==(const Sim_time& t1, const Sim_time& t2)
{
    return t1.milliseconds() == t2.milliseconds();
}

constexpr bool operator<(const Sim_time& t1, const Sim_time& t2)
{
    return t1.milliseconds() < t2.milliseconds();
}

constexpr bool operator<=(const Sim_time& t1, const Sim_time& t2)
{
    return t1.milliseconds() <= t2.milliseconds();
}

constexpr bool operator>(const Sim_time& t1, const Sim_time& t2)
{
    return t1.milliseconds() > t2.milliseconds();
}

constexpr bool operator>=(const Sim_time& t1, const Sim_time& t2)
{
    return t1.milliseconds() >= t2.milliseconds();
}

#endif // WOW_SIMULATOR_SIM_TIME_HPP
