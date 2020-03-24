#include <iostream>
#include <ctime>
#include <algorithm>

#include "wow_library/include/Combat_simulator.hpp"
#include "wow_library/include/Character.hpp"
#include "wow_library/include/Enchant.hpp"
#include "wow_library/include/Item.hpp"
#include "wow_library/include/Armory.hpp"

inline double normalCDF(double value)
{
    return 0.5 * erfc(-value * M_SQRT1_2);
}

double find_cdf_quantile(double target_quantile, double precision)
{
    double x = 0.0;
    double quantile = 0.5;
    while (quantile < target_quantile)
    {
        x += precision;
        quantile = normalCDF(x);
    }
    return x;
}

std::vector<double> find_cdf_quantile(const std::vector<double> &target_quantiles, double precision)
{
    std::vector<double> x_values;
    x_values.reserve(target_quantiles.size());
    for (const auto &quantile : target_quantiles)
    {
        x_values.emplace_back(find_cdf_quantile(quantile, precision));
    }
    return x_values;
}

class Item_optimizer
{
public:
    struct Sim_result_t
    {
        Sim_result_t() : character{Character(Race::human)}
        {
            mean_dps = 0;
            sample_std_dps = 0;
        }

        Sim_result_t(Character character, double mean_dps, double sample_std_dps) :
                character{std::move(character)},
                mean_dps{mean_dps},
                sample_std_dps{sample_std_dps} {}

        Character character;
        double mean_dps;
        double sample_std_dps;
    };

    static std::vector<std::vector<Weapon>> get_weapon_combinations(const std::vector<Weapon> &weapons);

    std::vector<std::vector<Armor>> get_combinations(const std::vector<Armor> &armors);

    static void add_enchants(Character &character);

    static void add_buffs(Character &character, const Buffs &buffs);

    std::vector<Sim_result_t> item_setup();
};

bool operator<(const Item_optimizer::Sim_result_t &left, const Item_optimizer::Sim_result_t &right)
{
    return left.mean_dps < right.mean_dps;
}

std::vector<std::vector<Weapon>> Item_optimizer::get_weapon_combinations(const std::vector<Weapon> &weapons)
{
    std::vector<Weapon> main_hands;
    std::vector<Weapon> one_hands;
    std::vector<Weapon> off_hands;
    std::vector<std::vector<Weapon>> weapon_combinations;
    for (const auto &wep : weapons)
    {
        switch (wep.get_socket())
        {
            case Weapon::Socket::main_hand:
                main_hands.emplace_back(wep);
                break;
            case Weapon::Socket::one_hand:
                one_hands.emplace_back(wep);
                break;
            case Weapon::Socket::off_hand:
                off_hands.emplace_back(wep);
                break;
        }
    }

    for (const auto &main_wep : main_hands)
    {
        for (const auto &one_wep : one_hands)
        {
            weapon_combinations.emplace_back(std::vector<Weapon>{main_wep, one_wep});
        }
        for (const auto &off_wep : off_hands)
        {
            weapon_combinations.emplace_back(std::vector<Weapon>{main_wep, off_wep});
        }
    }

    for (const auto &one_wep : one_hands)
    {
        for (const auto &off_wep : off_hands)
        {
            weapon_combinations.emplace_back(std::vector<Weapon>{one_wep, off_wep});
        }
    }

    // TODO unique tag needed here!!
    for (size_t i_1 = 0; i_1 < one_hands.size(); i_1++)
    {
        for (size_t i_2 = 0; i_2 < one_hands.size(); i_2++)
        {
            if (i_2 != i_1)
            {
                weapon_combinations.emplace_back(std::vector<Weapon>{one_hands[i_1], one_hands[i_2]});
            }
        }
    }

    return weapon_combinations;
}

std::vector<std::vector<Armor>> Item_optimizer::get_combinations(const std::vector<Armor> &armors)
{
    std::vector<std::vector<Armor>> combinations;
    for (size_t i_1 = 0; i_1 < armors.size() - 1; i_1++)
    {
        for (size_t i_2 = i_1 + 1; i_2 < armors.size(); i_2++)
        {
            combinations.emplace_back(std::vector<Armor>{armors[i_1], armors[i_2]});
        }
    }
    return combinations;
}

std::vector<Item_optimizer::Sim_result_t> Item_optimizer::item_setup()
{
    Armory armory;

    std::vector<Armor> helmets;
    helmets = {armory.helmet.lionheart_helm};

    std::vector<Armor> necks;
    necks = {
            armory.neck.onyxia_tooth_pendant
//            armory.neck.aq_barbed_choker
    };

    std::vector<Armor> shoulders;
    shoulders = {
//            armory.shoulder.drake_talon_pauldrons,
//            armory.shoulder.aq_chitinous_shoulderguards,
//            armory.shoulder.big_bad_pauldrons,
            armory.shoulder.truestrike_shoulders,
            armory.shoulder.black_dragonscale_shoulders
    };

    std::vector<Armor> backs;
    backs = {
//            armory.back.cloak_of_draconic_might,
            armory.back.shadewood_cloak
    };

    std::vector<Armor> chests;
    chests = {
//            armory.chest.savage_gladiator_chain,
            armory.chest.breastplate_of_bloodthirts,
            armory.chest.black_dragonscale_chest,
//            armory.chest.knight_captains_plate_hauberk,
//            armory.chest.tombstone_breastplate
//            armory.chest.aq_conquerors_breastplate,
//            armory.chest.aq_vest_of_swift_execution
    };

    std::vector<Armor> wrists;
    wrists = {
            armory.wrist.slashclaw_bracers,
            armory.wrist.battleborn_armbraces,
            armory.wrist.vambraces_of_the_sadist,
//            armory.wrist.wristguards_of_stability,
//            armory.wrist.battleborn_armbraces,
//            armory.wrist.aq_quiraji_execution_bracers
    };

    std::vector<Armor> hands;
    hands = {
            armory.hands.devilsaur_gauntlets,
//            armory.hands.devilsaur_gauntlets,
    };

    std::vector<Armor> belts;
    belts = {
            armory.belt.omokks_girth,
//            armory.belt.mugglers_belt
    };

    std::vector<Armor> legs;
    legs = {
//            armory.legs.aq_conquerors_legguards,
            armory.legs.devilsaur_leggings,
            armory.legs.black_dragonscale_leggings,
//            armory.legs.titanic_leggings
    };

    std::vector<Armor> boots;
    boots = {
            armory.boots.battlechasers,
            armory.boots.bloodmail_boots,
            armory.boots.black_dragonscale_boots,
    };

    std::vector<Armor> ranged;
    ranged = {
            armory.ranged.riphook,
            armory.ranged.satyrs_bow,
            armory.ranged.bloodseeker,
//            armory.ranged.precisely_calibrated_boomstick
    };

    std::vector<Armor> rings;
    rings = {
            armory.rings.quick_strike_ring,
            armory.rings.magnis_will,
//            armory.rings.blackstone_ring,
            armory.rings.tarnished_elven
    };

    std::vector<Armor> trinkets;
    trinkets = {
            armory.trinket.diamond_flask,
            armory.trinket.hand_of_justice,
//            armory.trinket.drake_fang_talisman,
            armory.trinket.blackhands_breadth,
    };

    std::vector<Weapon> weapons;
    weapons = {
            armory.swords.quel_serrar,
//            armory.daggers.core_hound_tooth,
//            armory.maces.stormstike_hammer,
            armory.swords.mirahs_song,
    };

    size_t n_helmet = helmets.size();
    size_t n_neck = necks.size();
    size_t n_shoulder = shoulders.size();
    size_t n_back = backs.size();
    size_t n_chest = chests.size();
    size_t n_wrist = wrists.size();
    size_t n_hands = hands.size();
    size_t n_belt = belts.size();
    size_t n_legs = legs.size();
    size_t n_boots = boots.size();
    size_t n_ranged = ranged.size();
    std::vector<std::vector<Weapon>> weapon_combinations = get_weapon_combinations(weapons);
    std::vector<std::vector<Armor>> ring_combinations = get_combinations(rings);
    std::vector<std::vector<Armor>> trinket_combinations = get_combinations(trinkets);
    size_t n_rings = ring_combinations.size();
    size_t n_trinket = trinket_combinations.size();
    size_t n_weapons = weapon_combinations.size();

    size_t combinations = n_helmet * n_neck * n_shoulder * n_back * n_chest * n_wrist * n_hands *
                          n_belt * n_legs * n_boots * n_ranged * n_rings * n_trinket * n_weapons;

    std::vector<Sim_result_t> characters;
    characters.reserve(combinations);
    for (size_t i_helm = 0; i_helm < n_helmet; i_helm++)
    {
        for (size_t i_neck = 0; i_neck < n_neck; i_neck++)
        {
            for (size_t i_shoulder = 0; i_shoulder < n_shoulder; i_shoulder++)
            {
                for (size_t i_back = 0; i_back < n_back; i_back++)
                {
                    for (size_t i_chest = 0; i_chest < n_chest; i_chest++)
                    {
                        for (size_t i_wrist = 0; i_wrist < n_wrist; i_wrist++)
                        {
                            for (size_t i_hands = 0; i_hands < n_hands; i_hands++)
                            {
                                for (size_t i_belt = 0; i_belt < n_belt; i_belt++)
                                {
                                    for (size_t i_legs = 0; i_legs < n_legs; i_legs++)
                                    {
                                        for (size_t i_boots = 0; i_boots < n_boots; i_boots++)
                                        {
                                            for (size_t i_ranged = 0; i_ranged < n_ranged; i_ranged++)
                                            {
                                                for (size_t i_ring = 0; i_ring < n_rings; i_ring++)
                                                {
                                                    for (size_t i_trink = 0; i_trink < n_trinket; i_trink++)
                                                    {
                                                        for (size_t i_wep = 0; i_wep < n_weapons; ++i_wep)
                                                        {
                                                            Character character{Race::human};

                                                            character.equip_armor(
                                                                    helmets[i_helm],
                                                                    necks[i_neck],
                                                                    shoulders[i_shoulder],
                                                                    backs[i_back],
                                                                    chests[i_chest],
                                                                    wrists[i_wrist],
                                                                    hands[i_hands],
                                                                    belts[i_belt],
                                                                    legs[i_legs],
                                                                    boots[i_boots],
                                                                    ranged[i_ranged],
                                                                    ring_combinations[i_ring][0],
                                                                    ring_combinations[i_ring][1],
                                                                    trinket_combinations[i_trink][0],
                                                                    trinket_combinations[i_trink][1]
                                                                                 );

                                                            character.equip_weapon(
                                                                    weapon_combinations[i_wep][0],
                                                                    weapon_combinations[i_wep][1]
                                                                                  );

                                                            character.check_if_armor_valid();
                                                            character.check_if_weapons_valid();

                                                            characters.emplace_back(character, 0, 0);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    assert(characters.size() == combinations);
    return characters;
}

void Item_optimizer::add_enchants(Character &character)
{
    character.add_enchants(Enchant{Enchant::Socket::head, Enchant::Type::haste},
                           Enchant{Enchant::Socket::back, Enchant::Type::agility},
                           Enchant{Enchant::Socket::chest, Enchant::Type::major_stats},
                           Enchant{Enchant::Socket::wrists, Enchant::Type::strength9},
                           Enchant{Enchant::Socket::hands, Enchant::Type::haste},
                           Enchant{Enchant::Socket::legs, Enchant::Type::haste},
                           Enchant{Enchant::Socket::weapon_mh, Enchant::Type::crusader},
                           Enchant{Enchant::Socket::weapon_oh, Enchant::Type::crusader}
                          );
}

void Item_optimizer::add_buffs(Character &character, const Buffs &buffs)
{
    character.add_buffs(
            buffs.rallying_cry,
            buffs.dire_maul,
//            buffs.songflower,
            buffs.blessing_of_kings,
            buffs.blessing_of_might,
            buffs.gift_of_the_wild,
            buffs.trueshot_aura,
            buffs.elixir_mongoose,
            buffs.dense_stone_mh,
            buffs.dense_stone_oh,
//            buffs.elemental_stone_mh,
//            buffs.elemental_stone_oh,
            buffs.blessed_sunfruit,
            buffs.juju_power,
            buffs.juju_might,
            buffs.roids
                       );
}

int main()
{
    Buffs buffs;
    Armory armory;
    Item_optimizer item_optimizer;

    auto characters = item_optimizer.item_setup();

    // Combat settings
    std::vector<int> batches_per_iteration = {20, 100, 1000, 10000};
    double sim_time = 60;
    int opponent_level = 63;

    Combat_simulator simulator = Combat_simulator();

    simulator.use_fast_but_sloppy_rng(); // Use before set seed!
//    simulator.set_seed(100); // Use for predictable random numbers
//    simulator.enable_rng_melee(); // Uses random swing damage instead of average
    simulator.enable_spell_rotation();
//    simulator.use_heroic_spamm();
    simulator.use_mighty_rage_potion();
    simulator.enable_anger_management();
    simulator.enable_bloodrage();
//    simulator.fuel_extra_rage(2.0, 500);
    simulator.enable_talents();
    simulator.enable_item_chance_on_hit_effects();
    simulator.enable_crusader();
    simulator.enable_death_wish();
//    simulator.enable_recklessness();
//    simulator.display_combat_debug();
    for (auto &sim_character : characters)
    {
        item_optimizer.add_enchants(sim_character.character);
        item_optimizer.add_buffs(sim_character.character, buffs);
        sim_character.character.compute_all_stats(Character::Talent::fury, armory.get_set_bonuses());
    }

    size_t n = characters.size();
    std::vector<size_t> keepers;
    for (size_t j = 0; j < n; ++j)
    {
        keepers.emplace_back(j);
    }

    for (size_t i = 0; i < batches_per_iteration.size(); i++)
    {
        clock_t startTime = clock();
        std::cout << "Iteration " << i + 1 << " of " << batches_per_iteration.size() << "\n";
        std::cout << "total_keepers: " << n << "\n";
        double best_dps = 0;
        double best_dps_std = 0;
        size_t iter = 0;
        for (auto &idx : keepers)
        {
            auto dps_vector = simulator
                    .simulate(characters[idx].character, sim_time, opponent_level, batches_per_iteration[i]);
            characters[idx].mean_dps = Combat_simulator::average(dps_vector);
            auto std_dps = Combat_simulator::standard_deviation(dps_vector, characters[idx].mean_dps);
            characters[idx].sample_std_dps = Combat_simulator::sample_deviation(std_dps, batches_per_iteration[i]);
            if (characters[idx].mean_dps > best_dps)
            {
                best_dps = characters[idx].mean_dps;
                best_dps_std = characters[idx].sample_std_dps;
            }
            iter++;
            if (keepers.size() < 200)
            {
                if (iter == 2)
                {
                    double time_spent = double(clock() - startTime) / (double) CLOCKS_PER_SEC;
                    double n_samples = keepers.size() / double(iter);
                    std::cout << "Batch done in: " << time_spent * n_samples << " seconds.";
                }
            }
            else
            {
                if (iter == 10)
                {
                    double time_spent = double(clock() - startTime) / (double) CLOCKS_PER_SEC;
                    double n_samples = keepers.size() / double(iter);
                    std::cout << "Batch done in: " << time_spent * n_samples << " seconds.";
                }
            }
        }

        double quantile = find_cdf_quantile(1 - 1 / static_cast<double>(n), 0.01);
        double filter_value = best_dps - quantile * best_dps_std;
        std::cout << "Best combination DPS: " << best_dps << ", deleting sets below: "
                  << best_dps - quantile * best_dps_std << "\n";

        std::vector<size_t> temp_keepers;
        temp_keepers.reserve(n);
        for (const size_t &index : keepers)
        {
            if (characters[index].mean_dps > filter_value)
            {
                temp_keepers.emplace_back(index);
            }
        }
        keepers = temp_keepers;
        n = keepers.size();
        if (n == 1)
        {
            break;
        }
    }

    std::cout << "\n" << "Printing best item sets:" << "\n";
    std::vector<Item_optimizer::Sim_result_t> best_characters;
    best_characters.reserve(keepers.size());
    for (const size_t &index: keepers)
    {
        best_characters.emplace_back(characters[index]);
    }
    std::sort(best_characters.begin(), best_characters.end());
    std::reverse(best_characters.begin(), best_characters.end());
    for (size_t i = 0; i < best_characters.size(); i++)
    {
        std::cout << "Character #" << i << "\n";
        std::cout << "DPS: " << best_characters[i].mean_dps << "\n";
        std::cout << "std: " << best_characters[i].sample_std_dps << "\n";
        std::cout << best_characters[i].character << "\n\n";
    }
}

