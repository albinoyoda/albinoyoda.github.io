#ifndef WOW_SIMULATOR_HIT_EFFECT_HPP
#define WOW_SIMULATOR_HIT_EFFECT_HPP

class Hit_effect
{
public:
    enum class Type
    {
        none,
        extra_hit,
        stat_boost,
        damage,
    };

    Hit_effect() = default;

    Hit_effect(Type type, Attributes attribute_boost, Special_stats special_stats_boost, double damage, double duration,
               double probability) :
            type_(type),
            attribute_boost_(attribute_boost),
            special_stats_boost_(special_stats_boost),
            damage_(damage),
            duration_(duration),
            probability_(probability) {};

    constexpr Type get_type() const
    {
        return type_;
    }

    constexpr Attributes get_Attributes() const
    {
        return attribute_boost_;
    }

    const Special_stats &get_special_stats_boost_() const
    {
        return special_stats_boost_;
    }

    constexpr double get_damage() const
    {
        return damage_;
    }

    constexpr double get_time() const
    {
        return duration_;
    }

private:
    Type type_;
    Attributes attribute_boost_;
    Special_stats special_stats_boost_;
    double damage_;
    double duration_;
    double probability_;
};

#endif //WOW_SIMULATOR_HIT_EFFECT_HPP
