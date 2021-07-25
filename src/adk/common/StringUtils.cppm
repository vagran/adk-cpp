export module adk.common.StringUtils;

#include <adk/defs.h>

import std.core;

export namespace adk {

/** Returns string as by sprintf(). */
std::string
StringFormat(const char *msg, ...) FORMAT_PRINTF(1, 2);

/** @param preallocSize Hint for buffer pre-allocation size. */
std::string
StringFormat(size_t preallocSize, const char *msg, ...) FORMAT_PRINTF(2, 3);

std::string
StringFormatV(const char *msg, va_list args) FORMAT_PRINTF(1, 0);

std::string
StringFormatV(size_t preallocSize, const char *msg, va_list args) FORMAT_PRINTF(2, 0);

}
