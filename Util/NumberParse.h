#pragma once

#include <charconv>
#include <cmath>
#include <locale>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace util
{

// Exception-free numeric parsing with the same rules everywhere:
// surrounding whitespace is ignored, the whole input must be consumed (so
// "5junk" or an all-whitespace string is rejected), unsigned types reject a
// sign prefix, floating-point results must be finite, and out-of-range values
// fail instead of wrapping. Replaces the per-plugin parsing helpers
// (BiliBiliPlugin kept a local toNumber, WeiboClient wrapped every stoi in its
// own try block, HLS kept three raw std::from_chars variants) with one
// implementation.
//
// Integral types use std::from_chars. Floating point cannot: libc++ (the
// standard library AppleClang ships) only implements the integral overloads,
// so a floating-point std::from_chars call does not even compile there. Floats
// are therefore read through a stream imbued with the classic locale, which is
// independent of the process locale (contrary to std::strtod) and behaves the
// same on every platform.
//
// Returns false when value is empty or whitespace-only, malformed, has
// trailing characters, is non-finite for floating types, or overflows the
// target type; result is only written on success.
template <typename T>
bool parseNumber(std::string_view value, T& result)
{
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "parseNumber requires an arithmetic type");

    constexpr std::string_view whitespace = " \t\r\n\f\v";
    const auto begin = value.find_first_not_of(whitespace);
    if (begin == std::string_view::npos)
    {
        return false;
    }
    value = value.substr(begin, value.find_last_not_of(whitespace) - begin + 1);

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
        const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), parsed);
        if (error != std::errc{} || end != value.data() + value.size())
        {
            return false;
        }
        result = parsed;
        return true;
    }
    else
    {
        // A parse that stops before the end of the input (e.g. "5junk", "1.2.3")
        // leaves eofbit clear, and a malformed or out-of-range value sets
        // failbit, so both checks reject it.
        std::istringstream stream{std::string(value)};
        stream.imbue(std::locale::classic());

        T parsed{};
        stream >> parsed;
        if (stream.fail() || !stream.eof() || !std::isfinite(static_cast<double>(parsed)))
        {
            return false;
        }
        result = parsed;
        return true;
    }
}

}  // namespace util
