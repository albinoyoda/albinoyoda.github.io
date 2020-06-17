#include <Armory.hpp>
#include <Character.hpp>
#include <Combat_simulator.hpp>

#include "sim_interface.hpp"

#include <sstream>

#include <iostream>
#include <ctime>
#include <algorithm>


namespace
{
    bool find_string(const std::vector<std::string> &string_vec, const std::string &match_string)
    {
        for (const auto &string : string_vec)
        {
            if (string == match_string)
            {
                return true;
            }
        }
        return false;
    }

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

    Race get_race(const std::string &race)
    {
        if (race == "human")
        {
            return Race::human;
        }
        else if (race == "gnome")
        {
            return Race::gnome;
        }
        else if (race == "dwarf")
        {
            return Race::dwarf;
        }
        else if (race == "night_elf")
        {
            return Race::night_elf;
        }
        else if (race == "orc")
        {
            return Race::orc;
        }
        else if (race == "troll")
        {
            return Race::troll;
        }
        else if (race == "undead")
        {
            return Race::undead;
        }
        else if (race == "tauren")
        {
            return Race::tauren;
        }
        else
        {
            std::cout << "Race not found!!! picking human" << "\n";
            return Race::human;
        }
    }

    std::string string_with_precision(double amount, size_t precision)
    {
        std::ostringstream stream;
        stream << std::setprecision(precision) << amount;
        return stream.str();
    }
}

struct Item_popularity
{
    Item_popularity() = default;

    Item_popularity(std::string name, size_t counter) :
            name{std::move(name)},
            counter{counter} {}

    std::string name;
    size_t counter{};
};

bool operator<(const Item_popularity &left, const std::string &right)
{
    return left.name < right;
}

bool operator==(const Item_popularity &left, const std::string &right)
{
    return left.name == right;
}

class Item_optimizer
{
public:
    struct Sim_result_t
    {
        Sim_result_t() : character{Character(Race::human, 60)}
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

    std::vector<Sim_result_t>
    item_setup(const std::vector<std::string> &armor_vec, const std::vector<std::string> &weapons_vec);

    void extract_armors(const std::vector<std::string> &armor_vec);

    void extract_weapons(const std::vector<std::string> &weapon_vec);

    void fill_empty();

    std::vector<Armor> helmets;
    std::vector<Armor> necks;
    std::vector<Armor> shoulders;
    std::vector<Armor> backs;
    std::vector<Armor> chests;
    std::vector<Armor> wrists;
    std::vector<Armor> hands;
    std::vector<Armor> belts;
    std::vector<Armor> legs;
    std::vector<Armor> boots;
    std::vector<Armor> ranged;
    std::vector<Armor> rings;
    std::vector<Armor> trinkets;
    std::vector<Weapon> weapons;

private:
    Armory armory;
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
        switch (wep.weapon_socket)
        {
            case Weapon_socket::main_hand:
                main_hands.emplace_back(wep);
                break;
            case Weapon_socket::one_hand:
                one_hands.emplace_back(wep);
                break;
            case Weapon_socket::off_hand:
                off_hands.emplace_back(wep);
                break;
            default:
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

bool is_armor_valid(const std::string &name)
{
    return !(name.substr(0, 14) == "item_not_found");
}

void Item_optimizer::extract_armors(const std::vector<std::string> &armor_vec)
{
    for (const auto &armor_name : armor_vec)
    {
        for (;;)
        {
            auto armor = armory.find_armor(Socket::head, armor_name);
            if (is_armor_valid(armor.name))
            {
                helmets.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::neck, armor_name);
            if (is_armor_valid(armor.name))
            {
                necks.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::shoulder, armor_name);
            if (is_armor_valid(armor.name))
            {
                shoulders.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::back, armor_name);
            if (is_armor_valid(armor.name))
            {
                backs.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::chest, armor_name);
            if (is_armor_valid(armor.name))
            {
                chests.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::wrist, armor_name);
            if (is_armor_valid(armor.name))
            {
                wrists.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::hands, armor_name);
            if (is_armor_valid(armor.name))
            {
                hands.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::belt, armor_name);
            if (is_armor_valid(armor.name))
            {
                belts.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::legs, armor_name);
            if (is_armor_valid(armor.name))
            {
                legs.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::boots, armor_name);
            if (is_armor_valid(armor.name))
            {
                boots.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::ring, armor_name);
            if (is_armor_valid(armor.name))
            {
                rings.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::trinket, armor_name);
            if (is_armor_valid(armor.name))
            {
                trinkets.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::ranged, armor_name);
            if (is_armor_valid(armor.name))
            {
                ranged.emplace_back(armor);
                break;
            }
        }
    }
}

void Item_optimizer::extract_weapons(const std::vector<std::string> &weapon_vec)
{
    for (const auto &weapon_name : weapon_vec)
    {
        for (;;)
        {
            auto weapon = armory.find_weapon(weapon_name);
            if (is_armor_valid(weapon.name))
            {
                weapons.emplace_back(weapon);
                break;
            }
        }
    }
}

void Item_optimizer::fill_empty()
{
    if (helmets.empty())
    {
        helmets.emplace_back(armory.find_armor(Socket::head, "helmet"));
    }
    if (necks.empty())
    {
        necks.emplace_back(armory.find_armor(Socket::neck, "neck"));
    }
    if (shoulders.empty())
    {
        shoulders.emplace_back(armory.find_armor(Socket::shoulder, "shoulder"));
    }
    if (backs.empty())
    {
        backs.emplace_back(armory.find_armor(Socket::back, "back"));
    }
    if (chests.empty())
    {
        chests.emplace_back(armory.find_armor(Socket::chest, "chest"));
    }
    if (wrists.empty())
    {
        wrists.emplace_back(armory.find_armor(Socket::chest, "wrists"));
    }
    if (hands.empty())
    {
        hands.emplace_back(armory.find_armor(Socket::chest, "hands"));
    }
    if (belts.empty())
    {
        belts.emplace_back(armory.find_armor(Socket::chest, "belt"));
    }
    if (legs.empty())
    {
        legs.emplace_back(armory.find_armor(Socket::chest, "legs"));
    }
    if (boots.empty())
    {
        boots.emplace_back(armory.find_armor(Socket::chest, "boots"));
    }
    if (ranged.empty())
    {
        ranged.emplace_back(armory.find_armor(Socket::chest, "ranged"));
    }

}

std::vector<Item_optimizer::Sim_result_t>
Item_optimizer::item_setup(const std::vector<std::string> &armor_vec, const std::vector<std::string> &weapons_vec)
{
    extract_armors(armor_vec);
    extract_weapons(weapons_vec);
    fill_empty();

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
                                                            Character character{Race::human, 60};

                                                            character.equip_armor(helmets[i_helm]);
                                                            character.equip_armor(necks[i_neck]);
                                                            character.equip_armor(shoulders[i_shoulder]);
                                                            character.equip_armor(backs[i_back]);
                                                            character.equip_armor(chests[i_chest]);
                                                            character.equip_armor(wrists[i_wrist]);
                                                            character.equip_armor(hands[i_hands]);
                                                            character.equip_armor(belts[i_belt]);
                                                            character.equip_armor(legs[i_legs]);
                                                            character.equip_armor(boots[i_boots]);
                                                            character.equip_armor(ranged[i_ranged]);
                                                            character.equip_armor(ring_combinations[i_ring][0]);
                                                            character.equip_armor(ring_combinations[i_ring][1]);
                                                            character.equip_armor(trinket_combinations[i_trink][0]);
                                                            character.equip_armor(trinket_combinations[i_trink][1]);

                                                            character.equip_weapon(weapon_combinations[i_wep][0],
                                                                                   weapon_combinations[i_wep][1]
                                                                                  );
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

void add_enchants_and_buffs(
        std::vector<Item_optimizer::Sim_result_t> &sim_results,
        const Race race,
        const Buffs &buffs,
        const std::vector<std::string> &buffs_vec,
        const std::vector<std::string> &ench_vec
                           )
{
    Armory armory;
    for (auto &sim_res : sim_results)
    {
        Character &character = sim_res.character;
        if (find_string(ench_vec, "e+8 strength"))
        {
            character.add_enchant(Socket::head, Enchant::Type::strength);
        }
        else if (find_string(ench_vec, "e+1 haste"))
        {
            character.add_enchant(Socket::head, Enchant::Type::haste);
        }

        if (find_string(ench_vec, "s+30 attack_power"))
        {
            character.add_enchant(Socket::shoulder, Enchant::Type::attack_power);
        }

        if (find_string(ench_vec, "b+3 agility"))
        {
            character.add_enchant(Socket::back, Enchant::Type::agility);
        }

        if (find_string(ench_vec, "c+3 stats"))
        {
            character.add_enchant(Socket::chest, Enchant::Type::minor_stats);
        }
        else if (find_string(ench_vec, "c+4 stats"))
        {
            character.add_enchant(Socket::chest, Enchant::Type::major_stats);
        }

        if (find_string(ench_vec, "w+7 strength"))
        {
            character.add_enchant(Socket::wrist, Enchant::Type::strength7);
        }
        else if (find_string(ench_vec, "w+9 strength"))
        {
            character.add_enchant(Socket::wrist, Enchant::Type::strength9);
        }

        if (find_string(ench_vec, "h+7 strength"))
        {
            character.add_enchant(Socket::hands, Enchant::Type::strength);
        }
        else if (find_string(ench_vec, "h+7 agility"))
        {
            character.add_enchant(Socket::hands, Enchant::Type::agility);
        }
        else if (find_string(ench_vec, "h+15 agility"))
        {
            character.add_enchant(Socket::hands, Enchant::Type::greater_agility);
        }
        else if (find_string(ench_vec, "h+1 haste"))
        {
            character.add_enchant(Socket::hands, Enchant::Type::haste);
        }

        if (find_string(ench_vec, "l+8 strength"))
        {
            character.add_enchant(Socket::legs, Enchant::Type::strength);
        }
        else if (find_string(ench_vec, "l+1 haste"))
        {
            character.add_enchant(Socket::legs, Enchant::Type::haste);
        }

        if (find_string(ench_vec, "b+7 agility"))
        {
            character.add_enchant(Socket::boots, Enchant::Type::agility);
        }

        if (find_string(ench_vec, "mcrusader"))
        {
            character.add_enchant(Socket::main_hand, Enchant::Type::crusader);
        }

        if (find_string(ench_vec, "ocrusader"))
        {
            character.add_enchant(Socket::off_hand, Enchant::Type::crusader);
        }

        // rallying cry
        if (find_string(buffs_vec, "rallying_cry"))
        {
            character.add_buff(buffs.rallying_cry);
        }
        if (find_string(buffs_vec, "dire_maul"))
        {
            character.add_buff(buffs.dire_maul);
        }
        if (find_string(buffs_vec, "songflower"))
        {
            character.add_buff(buffs.songflower);
        }
        if (find_string(buffs_vec, "warchiefs_blessing"))
        {
            character.add_buff(buffs.warchiefs_blessing);
        }
        if (find_string(buffs_vec, "spirit_of_zandalar"))
        {
            character.add_buff(buffs.spirit_of_zandalar);
        }
        if (find_string(buffs_vec, "sayges_fortune"))
        {
            character.add_buff(buffs.sayges_fortune);
        }

        // Player buffs
        if (find_string(buffs_vec, "battle_shout"))
        {
            character.add_buff(buffs.battle_shout);
        }
        if (find_string(buffs_vec, "blessing_of_kings"))
        {
            character.add_buff(buffs.blessing_of_kings);
        }
        if (find_string(buffs_vec, "blessing_of_might"))
        {
            character.add_buff(buffs.blessing_of_might);
        }
        if (find_string(buffs_vec, "windfury_totem"))
        {
            character.add_buff(buffs.windfury_totem);
        }
        if (find_string(buffs_vec, "strength_of_earth_totem"))
        {
            character.add_buff(buffs.strength_of_earth_totem);
        }
        if (find_string(buffs_vec, "grace_of_air_totem"))
        {
            character.add_buff(buffs.grace_of_air_totem);
        }
        if (find_string(buffs_vec, "gift_of_the_wild"))
        {
            character.add_buff(buffs.gift_of_the_wild);
        }
        if (find_string(buffs_vec, "leader_of_the_pack"))
        {
            character.add_buff(buffs.leader_of_the_pack);
        }
        if (find_string(buffs_vec, "trueshot_aura"))
        {
            character.add_buff(buffs.trueshot_aura);
        }
        if (find_string(buffs_vec, "elixir_mongoose"))
        {
            character.add_buff(buffs.elixir_mongoose);
        }
        if (find_string(buffs_vec, "blessed_sunfruit"))
        {
            character.add_buff(buffs.blessed_sunfruit);
        }
        if (find_string(buffs_vec, "smoked_dessert_dumplings"))
        {
            character.add_buff(buffs.smoked_dessert_dumplings);
        }
        if (find_string(buffs_vec, "juju_power"))
        {
            character.add_buff(buffs.juju_power);
        }
        if (find_string(buffs_vec, "elixir_of_giants"))
        {
            character.add_buff(buffs.elixir_of_giants);
        }
        if (find_string(buffs_vec, "juju_might"))
        {
            character.add_buff(buffs.juju_might);
        }
        if (find_string(buffs_vec, "winterfall_firewater"))
        {
            character.add_buff(buffs.winterfall_firewater);
        }
        if (find_string(buffs_vec, "roids"))
        {
            character.add_buff(buffs.roids);
        }
        if (find_string(buffs_vec, "mighty_rage_potion"))
        {
            character.add_buff(buffs.mighty_rage_potion);
        }
        if (find_string(buffs_vec, "dense_stone_main_hand"))
        {
            character.add_weapon_buff(Socket::main_hand, buffs.dense_stone);
        }
        else if (find_string(buffs_vec, "elemental_stone_main_hand"))
        {
            character.add_buff(buffs.elemental_stone);
        }
        if (find_string(buffs_vec, "dense_stone_off_hand"))
        {
            character.add_weapon_buff(Socket::off_hand, buffs.dense_stone);
        }
        else if (find_string(buffs_vec, "elemental_stone_off_hand"))
        {
            character.add_buff(buffs.elemental_stone);
        }

        armory.compute_total_stats(character);
    }
}

Sim_output_mult Sim_interface::simulate_mult(const Sim_input &input)
{
    Buffs buffs{};
    Item_optimizer item_optimizer;

    Race race = get_race(input.race[0]);
    std::vector<Item_optimizer::Sim_result_t> characters = item_optimizer
            .item_setup(input.mult_armor, input.mult_weapons);

    // Combat settings
    auto temp_buffs = input.buffs;

    if (find_string(input.options, "mighty_rage_potion"))
    {
        //temporary solution
        temp_buffs.emplace_back("mighty_rage_potion");
    }
    add_enchants_and_buffs(characters, race, buffs, temp_buffs, input.enchants);

    // Simulator & Combat settings
    Combat_simulator_config config{};
    config.n_batches = input.n_simulations;
    config.n_batches_statweights = input.n_simulations_stat_weights;
    config.sim_time = input.fight_time;
    config.opponent_level = input.target_level;

    if (find_string(input.options, "curse_of_recklessness"))
    {
        config.curse_of_recklessness_active = true;
    }
    if (find_string(input.options, "faerie_fire"))
    {
        config.faerie_fire_feral_active = true;
    }
    if (find_string(input.options, "death_wish"))
    {
        config.talents.death_wish = true;
    }
    if (find_string(input.options, "recklessness"))
    {
        config.enable_recklessness = true;
    }
    if (find_string(input.options, "sulfuron_harbinger"))
    {
        config.mode.sulfuron_harbinger = true;
    }
    if (find_string(input.options, "golemagg"))
    {
        config.mode.golemagg = true;
    }
    if (find_string(input.options, "vaelastrasz"))
    {
        config.mode.vaelastrasz = true;
    }
    if (find_string(input.options, "chromaggus"))
    {
        config.mode.chromaggus = true;
    }
    if (find_string(input.options, "use_bt_in_exec_phase"))
    {
        config.combat.use_bt_in_exec_phase = true;
    }
    if (find_string(input.options, "use_hs_in_exec_phase"))
    {
        config.combat.use_hs_in_exec_phase = true;
    }
    if (find_string(input.options, "cleave_if_adds"))
    {
        config.combat.cleave_if_adds = true;
    }
    if (find_string(input.options, "use_hamstring"))
    {
        config.combat.use_hamstring = true;
    }
    config.n_sunder_armor_stacks = input.sunder_armor;

    config.talents.improved_heroic_strike = 2;
    config.talents.unbridled_wrath = 5;
    config.talents.flurry = 5;
    config.talents.anger_management = true;
    config.talents.impale = 2;
    config.talents.improved_execute = 2;
    config.talents.dual_wield_specialization = 5;

    config.combat.heroic_strike_rage_thresh = input.heroic_strike_rage_thresh;
    config.combat.cleave_rage_thresh = input.cleave_rage_thresh;
    config.combat.whirlwind_rage_thresh = input.whirlwind_rage_thresh;
    config.combat.hamstring_cd_thresh = input.hamstring_cd_thresh;
    config.combat.hamstring_thresh_dd = input.hamstring_thresh_dd;
    config.combat.initial_rage = input.initial_rage;

    config.use_sim_time_ramp = true;
    config.enable_bloodrage = true;
    config.use_seed = true;
    config.seed = 110000;
    config.fuel_extra_rage = false;
    config.extra_rage_interval = 3;
    config.extra_rage_damage_amount = 150;

    Combat_simulator simulator(config);

    size_t n = characters.size();
    std::vector<size_t> keepers;
    for (size_t j = 0; j < n; ++j)
    {
        keepers.emplace_back(j);
    }

    std::vector<size_t> batches_per_iteration = {50, 200, 1000, 10000};
    size_t n_sim{};
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
            n_sim += batches_per_iteration[i];
            simulator.simulate(characters[idx].character, batches_per_iteration[i]);
            characters[idx].mean_dps = simulator.get_dps_mean();
            auto std_dps = std::sqrt(simulator.get_dps_variance());
            characters[idx].sample_std_dps = Statistics::sample_deviation(std_dps, batches_per_iteration[i]);
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
                    std::cout << "Batch done in: " << time_spent * n_samples << " seconds.\n";
                }
            }
        }

        double quantile = find_cdf_quantile(1 - 1 / static_cast<double>(10 * n), 0.01);
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
        if (n < 20)
        {
            break;
        }
        keepers = temp_keepers;
        n = keepers.size();
    }

    std::string message = +"<b>Total number of simulations performed: </b>" + std::to_string(n_sim) + "<br>";
    message += "<b>Best item set/sets <u>sorted</u> by DPS:</b><br>";
    message += "<b>Item formatting</b>: <br>[Item presence / total sets] - socket - item_name<br>";
    message += "('Better' item = higher presence.)<br><br>";
    std::vector<Item_optimizer::Sim_result_t> best_characters;
    best_characters.reserve(keepers.size());
    for (const size_t &index: keepers)
    {
        best_characters.emplace_back(characters[index]);
    }
    std::sort(best_characters.begin(), best_characters.end());
    std::reverse(best_characters.begin(), best_characters.end());

    std::vector<std::vector<Item_popularity>> item_popularity;
    item_popularity.reserve(best_characters.size());
    for (size_t j = 0; j < best_characters[0].character.armor.size(); j++)
    {
        Item_popularity a{"none", 0};
        item_popularity.push_back({a});
    }
    std::vector<std::vector<Item_popularity>> weapon_popularity;
    weapon_popularity.reserve(best_characters.size());
    for (size_t j = 0; j < 2; j++)
    {
        Item_popularity a{"none", 0};
        item_popularity.push_back({a});
    }
    for (auto &best_character : best_characters)
    {
        for (size_t j = 0; j < best_character.character.armor.size(); j++)
        {
            bool found = false;
            for (auto &item : item_popularity[j])
            {
                if (item.name == best_character.character.armor[j].name)
                {
                    item.counter++;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                Item_popularity a{best_character.character.armor[j].name, 1};
                item_popularity[j].push_back(a);
            }
        }
        for (size_t j = 0; j < 2; j++)
        {
            bool found = false;
            for (auto &item : weapon_popularity[j])
            {
                if (item.name == best_character.character.weapons[j].name)
                {
                    item.counter++;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                Item_popularity a{best_character.character.weapons[j].name, 1};
                weapon_popularity[j].push_back(a);
            }
        }
    }

    std::vector<std::string> socket_names = {"Helmet", "Neck", "Shoulder", "Back", "Chest", "Bracers", "Hands", "Belt",
                                             "Legs", "Boots", "Ranged", "Ring 1", "Ring 2", "Trinket 1", "Trinket 2"};

    std::vector<std::string> weapon_names = {"Main-hand", "Off-hand"};

    for (size_t i = 0; i < best_characters.size(); i++)
    {
        message += "<b>Set " + std::to_string(i + 1) + ":</b><br>";
        message += "DPS: " + string_with_precision(best_characters[i].mean_dps, 5) + "<br>";
        message += "Error margin: " + string_with_precision(best_characters[i].sample_std_dps, 3) + "<br>";
        message += "<b>Stats:</b><br>";
        message += "Hit: " + string_with_precision(best_characters[i].character.total_special_stats.hit, 3) + " %<br>";
        message += "Crit: " + string_with_precision(best_characters[i].character.total_special_stats.critical_strike,
                                                    3) + " %<br>";
        message += "Attackpower: " + string_with_precision(
                best_characters[i].character.total_special_stats.attack_power, 4) + " <br>";
        message += "<b>Armor:</b><br>";
        for (size_t j = 0; j < best_characters[i].character.armor.size(); j++)
        {
            auto item_pop = (*std::find(item_popularity[j].begin(), item_popularity[j].end(),
                                        best_characters[i].character.armor[j].name));
            message += "[" + std::to_string(item_pop.counter) + "/" + std::to_string(best_characters.size()) + "] ";
            message += "<b>" + socket_names[j] + "</b>" + " - " + best_characters[i].character.armor[j].name + "<br>";
        }
        message += "<b>Weapons:</b><br>";
        for (size_t j = 0; j < 2; j++)
        {
            auto item_pop = (*std::find(weapon_popularity[j].begin(), weapon_popularity[j].end(),
                                        best_characters[i].character.weapons[j].name));
            message += "[" + std::to_string(item_pop.counter) + "/" + std::to_string(best_characters.size()) + "] ";
            message += "<b>" + weapon_names[j] + "</b>" + " - " + best_characters[i].character.weapons[j].name + "<br>";
        }
        message += "<br>";
    }

    return {{message}};
}

