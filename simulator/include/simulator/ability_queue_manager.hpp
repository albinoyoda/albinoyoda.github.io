#ifndef WOW_SIMULATOR_ABILITY_QUEUE_MANAGER_HPP
#define WOW_SIMULATOR_ABILITY_QUEUE_MANAGER_HPP

class Ability_queue_manager
{
public:
    [[nodiscard]] constexpr bool is_ability_queued() const { return heroic_strike_queued || cleave_queued; }

    constexpr void queue_heroic_strike()
    {
        heroic_strike_queued = true;
        cleave_queued = false;
    }

    constexpr void queue_cleave()
    {
        heroic_strike_queued = false;
        cleave_queued = true;
    }

    constexpr void reset()
    {
        heroic_strike_queued = false;
        cleave_queued = false;
    }

    [[nodiscard]] constexpr bool is_heroic_strike_queued() const { return heroic_strike_queued; }

    [[nodiscard]] constexpr bool is_cleave_queued() const { return cleave_queued; }

private:
    bool heroic_strike_queued{false};
    bool cleave_queued{false};
};

#endif // WOW_SIMULATOR_ABILITY_QUEUE_MANAGER_HPP
