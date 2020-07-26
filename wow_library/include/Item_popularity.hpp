#ifndef WOW_SIMULATOR_ITEM_POPULARITY_HPP
#define WOW_SIMULATOR_ITEM_POPULARITY_HPP

struct Item_popularity
{
    Item_popularity() = default;

    Item_popularity(std::string name, size_t counter) : name{std::move(name)}, counter{counter} {}

    std::string name;
    size_t counter{};
};

bool operator<(const Item_popularity& left, const Item_popularity& right)
{
    return left.counter < right.counter;
}

bool operator==(const Item_popularity& left, const Item_popularity& right)
{
    return left.counter == right.counter;
}

bool operator<(const Item_popularity& left, const std::string& right)
{
    return left.name < right;
}

bool operator==(const Item_popularity& left, const std::string& right)
{
    return left.name == right;
}

#endif // WOW_SIMULATOR_ITEM_POPULARITY_HPP
