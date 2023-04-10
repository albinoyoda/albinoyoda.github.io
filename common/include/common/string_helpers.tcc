namespace String_helpers
{
template <typename T>
bool does_vector_contain(const std::vector<T>& vec, const T& match)
{
    for (const auto& elem : vec)
    {
        if (elem == match)
        {
            return true;
        }
    }
    return false;
}
} // namespace String_helpers
