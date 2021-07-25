module adk.common.StringUtils;

import std.core;

#include <adk/defs.h>


std::string
adk::StringFormat(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    std::string result = StringFormatV(std::strlen(msg) * 2, msg, args);
    va_end(args);
    return result;
}

std::string
adk::StringFormat(size_t preallocSize, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    std::string result = StringFormatV(preallocSize, msg, args);
    va_end(args);
    return result;
}

std::string
adk::StringFormatV(const char *msg, va_list args)
{
    return StringFormat(std::strlen(msg) * 2, msg, args);
}

std::string
adk::StringFormatV(size_t preallocSize, const char *msg, va_list args)
{
    va_list _args;
    va_copy(_args, args);
    std::string result(preallocSize, '\0');
    int n = vsnprintf(result.data(), preallocSize, msg, _args);
    va_end(_args);
    if (n < 0) {
        return "<FormatError>";
    }
    if (n < preallocSize) {
        result.resize(n);
        return result;
    }
    preallocSize = n + 1;
    result.resize(preallocSize, '\0');
    va_copy(_args, args);
    n = vsnprintf(&result.front(), preallocSize, msg, _args);
    va_end(_args);
    if (n < 0 || n >= preallocSize) {
        return "<FormatError>";
    }
    result.resize(n);
    return result;
}
