#include "simulator/spells.hpp"

namespace spellbook
{
Over_time_effect deep_wounds(double hit_damage)
{
    return {"Deep_wounds", {}, 0, hit_damage, Sim_time::from_seconds(3), Sim_time::from_seconds(12)};
}
} // namespace spellbook
