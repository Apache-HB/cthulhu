#pragma once

#include "core/compiler.h"

/// @defgroup Macros Common macros
/// @brief Common macros used throughout the project
/// @ingroup Core
/// @{

#ifdef __cplusplus
#   define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#else
#   define STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)
#endif

#if __cplusplus >= 201402L
#   define CTU_DEPRECATED(msg) [[deprecated(msg)]]
#elif CC_MSVC
#   define CTU_DEPRECATED(msg) __declspec(deprecated(msg))
#elif CC_CLANG || CC_GNU
#   define CTU_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
#   define CTU_DEPRECATED(msg)
#endif

/// @def CTU_DEPRECATED(msg)
/// @brief mark a function as deprecated
/// @param msg the message to display when the function is used

/// @def MAX(lhs, rhs)
/// returns the maximum of @a lhs and @a rhs
#define MAX(L, R) ((L) > (R) ? (L) : (R))

/// @def MIN(lhs, rhs)
/// returns the minimum of @a lhs and @a rhs
#define MIN(L, R) ((L) < (R) ? (L) : (R))

/// @def ALIGN_POW2(x, align)
/// aligns @p X to the next power of 2 of @p ALIGN
#define ALIGN_POW2(X, ALIGN) (((X) + (ALIGN)-1) & ~((ALIGN)-1))

#define CTU_UNUSED(x) ((void)(x))

#define INNER_STR(x) #x
#define STR(x) INNER_STR(x)

/// @defgroup Colour ANSI Colour macros
/// @brief ANSI escape string colour macros
/// @ingroup Core
///
/// Useful for formatting messages to the console.
/// @{

#define ANSI_RED "\x1B[1;31m"    ///< ANSI red
#define ANSI_GREEN "\x1B[1;32m"  ///< ANSI green
#define ANSI_YELLOW "\x1B[1;33m" ///< ANSI yellow
#define ANSI_BLUE "\x1B[1;34m"   ///< ANSI blue
#define ANSI_MAGENTA "\x1B[1;35m" ///< ANSI magenta
#define ANSI_CYAN "\x1B[1;36m"   ///< ANSI cyan
#define ANSI_WHITE "\x1B[1;37m"  ///< ANSI white

#define ANSI_RESET "\x1B[0m"     ///< ANSI reset

/// @}

/// @defgroup ErrorCodes Exit codes
/// @brief Exit codes that match with GNU standard codes
/// @ingroup Core
///
/// Used in tests and the cli.
/// @{

#define EXIT_SHOULD_EXIT 2 ///< the user has requested to exit the program
#define EXIT_OK 0        ///< no user errors or internal errors
#define EXIT_ERROR 1     ///< the user has made an error
#define EXIT_INTERNAL 99 ///< internal compiler errors have occurred

/// @}

/// @}
