#ifndef WOW_SIMULATOR_FLURRY_MANAGER_HPP
#define WOW_SIMULATOR_FLURRY_MANAGER_HPP

#include "hit_tables.hpp"

class Flurry_manager
{
public:
    Flurry_manager() = default;

    Flurry_manager(Special_stats& special_stats, const double flurry_haste_factor, const int flurry_charges,
                   const bool enabled)
        : special_stats_{&special_stats}
        , flurry_haste_factor_{flurry_haste_factor}
        , flurry_charges_{flurry_charges}
        , enabled_{enabled} {};

    void update(Melee_result hit_result)
    {
        if (enabled_)
        {
            bool flurry_active = (flurry_charges_ > 0);
            //            if (!is_ability)
            //            {
            //                flurry_charges--;
            //                flurry_charges = std::max(0, flurry_charges);
            //            }
            if (hit_result == Melee_result::crit)
            {
                flurry_charges_ = 3;
                if (!flurry_active)
                {
                    *special_stats_ += {0, 0, 0, 0, flurry_haste_factor_};
                }
            }
            else if (flurry_active && flurry_charges_ == 0)
            {
                *special_stats_ -= {0, 0, 0, 0, flurry_haste_factor_};
            }
            //            simulator_cout(flurry_charges, " flurry charges");
        }
    }

    [[nodiscard]] constexpr int flurry_charges() const { return flurry_charges_; }

    [[nodiscard]] constexpr bool has_flurry_charges() const { return flurry_charges_ > 0; }

private:
    Special_stats* special_stats_{};
    double flurry_haste_factor_{};
    int flurry_charges_{};
    bool enabled_{};
};

#endif // WOW_SIMULATOR_FLURRY_MANAGER_HPP
