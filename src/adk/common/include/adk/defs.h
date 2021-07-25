#ifndef ADK_COMMON_INCLUDE_ADK_DEFS_H_
#define ADK_COMMON_INCLUDE_ADK_DEFS_H_

#define CONCAT2(__x, __y)       __x##__y

/** Macro for concatenating identifiers. */
#define CONCAT(__x, __y)        CONCAT2(__x, __y)

#define STR2(__x)               # __x
/** Macro for stringifying identifiers. */
#define STR(__x)                STR2(__x)

#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(__array)   (sizeof(__array) / sizeof((__array)[0]))
#endif

/** Specify that a function has format arguments (like printf or scanf). See
 * 'format' attribute description in GCC documentation.
 */
#if defined(__GNUC__) || defined(__clang__)
#define FORMAT(__type, __fmt_idx, __arg_idx)    \
    __attribute__ ((format(__type, __fmt_idx, __arg_idx)))
#else
#define FORMAT(type, fmt_idx, arg_idx)
#endif /* __GNUC__ */

#ifndef _WIN32
#define FORMAT_PRINTF(__fmt_idx, __arg_idx) FORMAT(printf, __fmt_idx, __arg_idx)
#else
#ifndef __USE_MINGW_ANSI_STDIO
#error __USE_MINGW_ANSI_STDIO=1 must be defined via makefile
#endif
#define FORMAT_PRINTF(__fmt_idx, __arg_idx) FORMAT(__MINGW_PRINTF_FORMAT, __fmt_idx, __arg_idx)
#endif

/** Use with unused arguments if you like to declare that it is not (yet) used
 * a the function.
 */
#define UNUSED      __attribute__((unused))

/** Pack structure or class, i.e. do not allow the compiler to insert paddings
 * for members alignment.
 */
#define PACKED      __attribute__((packed))

/** Produce warning if the function result is discarded. */
#if defined(__GNUC__) || defined(__clang__)
#define WARN_UNUSED_RESULT      __attribute__((warn_unused_result))
#else /* __GNUC__ */
#define WARN_UNUSED_RESULT
#endif /* __GNUC__ */

/** Currently unable to include cstdarg in module files, so defining them here. */
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)
#define va_copy(dest, src)  __builtin_va_copy(dest, src)

#endif /* ADK_COMMON_INCLUDE_ADK_DEFS_H_ */
