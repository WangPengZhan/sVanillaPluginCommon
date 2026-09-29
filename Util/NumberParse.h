#pragma once

#include <charconv>
#include <string>
#include <string_view>
#include <type_traits>

namespace util
{

// Exception-free numeric parsing with the same rules everywhere:
// the whole input must be consumed, so "5junk" or an empty string is rejected,
// and out-of-range values fail instead of wrapping. Replaces the per-plugin
// try/catch stoi/stoll wrappers (BiliBiliPlugin kept a local toNumber,
// WeiboClient wrapped every stoi in its own try block, HLS used raw
// std::from_chars) with one implementation.
// Returns false when value is empty, malformed, has trailing characters or
// overflows the target type; result is only written on success.
template <typename T>
bool parseNumber(const std::string& value, T& result)
{
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "parseNumber requires an arithmetic type");

    if (value.empty())
    {
        return false;
    }

    const char* const first = value.data();
    const char* const last = value.data() + value.size();

    if constexpr (std::is_integral_v<T>)
    {
        if constexpr (std::is_unsigned_v<T>)
        {
            // std::from_chars accepts a leading '-' for unsigned types and
            // produces a wrapped value, so reject signs up front.
            if (value.front() == '-' || value.front() == '+')
            {
                return false;
            }
        }

        T parsed{};
        const auto [end, error] = std::from_chars(first, last, parsed);
        if (error != std::errc{} || end != last)
        {
            return false;
        }
        result = parsed;
        return true;
    }
    else
    {
        double parsed{};
        const auto [end, error] = std::from_chars(first, last, parsed);
        if (error != std::errc{} || end != last)
        {
            return false;
        }
        result = static_cast<T>(parsed);
        return true;
    }
}

}  // namespace util
