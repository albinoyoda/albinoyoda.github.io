#include "simulator/use_effects.hpp"

namespace spellbook
{
Use_effect berserking(double haste)
{
    return {"Berserking",
            Use_effect::Effect_socket::unique,
            {},
            {0, 0, 0, 0, haste},
            0,
            Sim_time::from_seconds(10),
            Sim_time::from_seconds(180),
            false};
}
} // namespace spellbook
