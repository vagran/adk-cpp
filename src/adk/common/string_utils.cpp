module;

#if defined( __has_include ) &&  __has_include(<cxxabi.h>)
#include <cxxabi.h>
#define HAS_CXXABI
#endif

module adk.common.string_utils;

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

std::string_view
adk::GetFileBaseName(const std::string_view &filePath)
{
    std::size_t lastSlash = filePath.find_last_of('/');
    if (lastSlash != std::string_view::npos) {
        return filePath.substr(lastSlash + 1);
    }
    return filePath;
}

#ifdef HAS_CXXABI

std::string
adk::DemangleName(const char *name)
{
    int status = 0;
    std::unique_ptr<char, void(*)(void*)> demangled(
        abi::__cxa_demangle(name, nullptr, nullptr, &status),
        std::free);
    return demangled ? demangled.get() : name;
}

#else /* HAS_CXXABI */

std::string
adk::DemangleName(const char *name)
{
    return name;
}

#endif /* HAS_CXXABI */
