export module adk.common;

import std.core;

/** Useful for casting negative marker values to size_t. */
export constexpr std::size_t operator "" _sz(unsigned long long value)
{
    return value;
}
