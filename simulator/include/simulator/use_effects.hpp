#ifndef WOW_SIMULATOR_USE_EFFECTS_HPP
#define WOW_SIMULATOR_USE_EFFECTS_HPP

#include "item.hpp"

namespace spellbook
{

const Use_effect deathwish = {"Death_wish",
                              Use_effect::Effect_socket::unique,
                              {},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, .20},
                              -10,
                              Sim_time::from_seconds(30),
                              Sim_time::from_seconds(180),
                              true};

const Use_effect recklessness = {"Recklessness",
                                 Use_effect::Effect_socket::unique,
                                 {},
                                 {100, 0, 0},
                                 0,
                                 Sim_time::from_seconds(15),
                                 Sim_time::from_seconds(900),
                                 true};

const Use_effect bloodrage = {"Bloodrage",
                              Use_effect::Effect_socket::unique,
                              {},
                              {},
                              10,
                              Sim_time::from_seconds(10),
                              Sim_time::from_seconds(60),
                              false,
                              {},
                              {{"Bloodrage", {}, 1, 0, Sim_time::from_seconds(1), Sim_time::from_seconds(10)}}};

Use_effect berserking(double haste);
} // namespace spellbook

#endif // WOW_SIMULATOR_USE_EFFECTS_HPP
