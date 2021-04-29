#include "item_upgrades_fixture.cpp"
#include "string_helpers.hpp"

size_t get_number_of_positive_upgrades(const std::vector<Item_upgrade_result>& item_upgrades)
{
    size_t n_positive = 0;
    for (const auto& upgrade : item_upgrades)
    {
        if (upgrade.second.mean_ > 0.0)
        {
            n_positive++;
        }
    }
    return n_positive;
}

size_t get_number_of_positive_upgrades_w_confidence(const std::vector<Item_upgrade_result>& item_upgrades)
{
    size_t n_positive = 0;
    for (const auto& upgrade : item_upgrades)
    {
        auto conf_interval = upgrade.second.confidence_interval(0.95);
        if (conf_interval.first > 0.0)
        {
            n_positive++;
        }
    }
    return n_positive;
}

TEST_F(Item_upgrades_fixture, test_all_upgrades_positive_when_using_item_without_stats)
{
    auto item_upgrades = get_item_upgrades(Socket::head, 5);
    size_t n_positive = get_number_of_positive_upgrades(item_upgrades);

    EXPECT_GT(item_upgrades.size(), 5);
    EXPECT_EQ(n_positive, item_upgrades.size());
}

TEST_F(Item_upgrades_fixture, test_no_upgrades_when_using_strong_item)
{
    add_item_to_character("lionheart_helm", Socket::head);
    auto item_upgrades = get_item_upgrades(Socket::head, 5);

    size_t n_positive = get_number_of_positive_upgrades(item_upgrades);
    EXPECT_TRUE(n_positive == 0);
}

TEST_F(Item_upgrades_fixture, test_upgrades_rings)
{
    add_item_to_character("quick_strike_ring", Socket::ring, true);
    add_item_to_character("masons_fraternity_ring", Socket::ring, false);
    auto item_upgrades_quick_strike = get_item_upgrades(Socket::ring, 5, true);
    auto item_upgrades_mason = get_item_upgrades(Socket::ring, 5, false);

    size_t n_positive_qs = get_number_of_positive_upgrades_w_confidence(item_upgrades_quick_strike);
    size_t n_positive_m = get_number_of_positive_upgrades_w_confidence(item_upgrades_mason);

    EXPECT_TRUE(n_positive_qs < n_positive_m);
}

TEST_F(Item_upgrades_fixture, test_upgrades_trinket)
{
    add_item_to_character("mark_of_the_champion", Socket::trinket, true);
    add_item_to_character("darkmoon_card_maelstrom", Socket::trinket, false);
    auto item_upgrades_strong = get_item_upgrades(Socket::trinket, 5, true);
    auto item_upgrades_weak = get_item_upgrades(Socket::trinket, 5, false);

    size_t n_positive_strong = get_number_of_positive_upgrades_w_confidence(item_upgrades_strong);
    size_t n_positive_weak = get_number_of_positive_upgrades_w_confidence(item_upgrades_weak);

    EXPECT_TRUE(n_positive_strong < n_positive_weak);
}