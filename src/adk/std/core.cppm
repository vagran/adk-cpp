export module std.core;

export import std_config;

export import <cstddef>;
export import <cstdint>;
export import <cstdlib>;
export import <cstdint>;
export import <cstdlib>;

export import <utility>;
export import <type_traits>;

export import <exception>;
export import <stdexcept>;
export import <system_error>;

export import <new>;
export import <memory>;

export import <limits>;
export import <ctime>;
export import <atomic>;
export import <chrono>;
export import <bitset>;

export import <iterator>;
export import <algorithm>;
export import <functional>;

export import <string>;
export import <cstring>;

export import <array>;
export import <vector>;
export import <set>;
export import <map>;
export import <unordered_set>;
export import <unordered_map>;
export import <list>;
export import <forward_list>;
export import <stack>;

export import <concepts>;

#include "source_location.h"

#if defined(__has_include)
#   if __has_include(<coroutine>)
        export import <coroutine>;
#   elif __has_include(<experimental/coroutine>)
        export import <experimental/coroutine>;
#   endif
#else
    export import <coroutine>;
#endif
