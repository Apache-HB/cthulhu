#pragma once

#include "core/compiler.h"
#include "core/analyze.h"

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

BEGIN_API

/// @defgroup Stacktrace Stacktrace library
/// @brief Stacktrace library
/// @ingroup Common
/// @{

#define STACKTRACE_NAME_LENGTH 256
#define STACKTRACE_PATH_LENGTH 1024

/// @brief a symbol
typedef struct symbol_t
{
    /// @brief the line number
    size_t line;

    /// @brief the symbol name
    char name[STACKTRACE_NAME_LENGTH];

    /// @brief the file path
    char file[STACKTRACE_PATH_LENGTH];
} symbol_t;

/// @brief a stacktrace frame
typedef struct frame_t
{
    /// @brief the frame address
    uintptr_t address;
} frame_t;

typedef enum frame_resolve_t
{
    /// @brief nothing was resolved
    eResolveNothing         = (0),

    /// @brief the line number was found
    /// @note this does not imply that the file was found
    eResolveLine            = (1 << 0),

    /// @brief the symbol name was found
    /// @note this does not imply it was demangled
    eResolveName            = (1 << 1),

    /// @brief the symbol name was demangled
    eResolveDemangledName   = (1 << 2),

    /// @brief the file path was found
    eResolveFile            = (1 << 3),

    eResolveCount
} frame_resolve_t;

typedef void (*bt_frame_t)(void *user, const frame_t *frame);

/// @brief initialize the stacktrace backend
/// @note this function must be called before any other stacktrace function
void stacktrace_init(void);

/// @brief get the stacktrace backend name
/// @return the stacktrace backend name
RET_STRING
const char *stacktrace_backend(void);

/// @brief get a stacktrace from the current location
/// @note this function is not thread safe
/// @note @ref stacktrace_init must be called before calling this function
///
/// @param frames the frames to fill
/// @param size the number of frames @p frames can hold
///
/// @return the number of frames filled
RET_RANGE(<=, size)
size_t stacktrace_get(
        frame_t *frames, // OUT_WRITES(return)
        IN_RANGE(>, 0) size_t size);

/// @brief get a stacktrace from the current location using a callback
/// @note this function is not thread safe
/// @note @ref stacktrace_init must be called before calling this function
///
/// @param callback the callback to call for each frame
/// @param user the user data to pass to the callback
void stacktrace_read(bt_frame_t callback, void *user);

/// @brief resolve a frame to a symbol
///
/// @note @ref stacktrace_init must be called before calling this function
///
/// @param frame the frame to resolve
/// @param symbol the symbol to fill
frame_resolve_t frame_resolve(IN_NOTNULL const frame_t *frame, symbol_t *symbol);

/// @brief print a stacktrace to a file
/// @note this follows the same precondition as @ref stacktrace_get
///
/// @param file the file to print to
void stacktrace_print(IN_NOTNULL FILE *file);

/// @}

END_API
