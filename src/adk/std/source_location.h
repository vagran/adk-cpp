#ifndef ADK_STD_SOURCE_LOCATION_H_
#define ADK_STD_SOURCE_LOCATION_H_

/* Implementation of std::source_location until supported in libc++. */

export namespace std {

struct source_location {
public:
    source_location() = default;

    static source_location
    current(std::uint_least32_t line = __builtin_LINE(),
            std::uint_least32_t column = __builtin_COLUMN(),
            const char *file_name = __builtin_FILE(),
            const char *function_name = __builtin_FUNCTION()) noexcept
    {
        return source_location(line, column, file_name, function_name);
    }

    constexpr std::uint_least32_t
    line() const noexcept
    {
        return _line;
    }

    constexpr std::uint_least32_t
    column() const noexcept
    {
        return _column;
    }

    constexpr const char *
    file_name() const noexcept
    {
        return _file_name;
    }

    constexpr const char *
    function_name() const noexcept
    {
        return _function_name;
    }

private:
    std::uint_least32_t _line = 0;
    std::uint_least32_t _column = 0;
    const char *_file_name = nullptr;
    const char *_function_name = nullptr;

    constexpr source_location(std::uint_least32_t line,
                              std::uint_least32_t column,
                              const char *file_name,
                              const char *function_name):
       _line(line),
       _column(column),
       _file_name(file_name),
       _function_name(function_name)
    {}
};

} /* namespace std */

#endif /* ADK_STD_SOURCE_LOCATION_H_ */
