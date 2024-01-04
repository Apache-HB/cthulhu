#pragma once

#include "core/analyze.h"
#include "core/compiler.h"

#include <stddef.h>

typedef struct arena_t arena_t;

BEGIN_API

/// @defgroup TypedVector Typed vector
/// @ingroup Standard
/// @brief Generic vector of typed values
/// @{

typedef struct typevec_t typevec_t;

/// @brief create a new typed vector
///
/// @param type_size the size of the type
/// @param len the initial length of the vector
/// @param arena the arena to allocate from
///
/// @return the new vector
NODISCARD
typevec_t *typevec_new(IN_RANGE(>, 0) size_t type_size, size_t len, arena_t *arena);

/// @brief create a new typed vector with an initial size and length
/// @note it is expected that the user will fill the vector up to @a len using @a typevec_set
///       with valid values rather than using @a typevec_push
///
/// @param type_size the size of the type
/// @param len the initial length of the vector
///
/// @return the new vector
NODISCARD
typevec_t *typevec_of(IN_RANGE(>, 0) size_t type_size, size_t len);

/// @brief create a new typed vector with an initial first value
///
/// @param type_size the size of the type
/// @param value the initial value
///
/// @return the new vector
NODISCARD
typevec_t *typevec_init(IN_RANGE(>, 0) size_t type_size, IN_NOTNULL const void *value);

/// @brief get the length of a vector
///
/// @param vec the vector to get the length of
/// @return the length of the vector
NODISCARD
size_t typevec_len(IN_NOTNULL const typevec_t *vec);

/// @brief set an element in the vector
///
/// @param vec the vector to set the value in
/// @param index the index to set the value at
/// @param src the value to set
void typevec_set(IN_NOTNULL typevec_t *vec, size_t index, IN_NOTNULL const void *src);

/// @brief get an element from the vector
///
/// @param vec the vector to get the value from
/// @param index the index to get the value from
/// @param dst the destination to copy the value to
void typevec_get(IN_NOTNULL const typevec_t *vec, size_t index, IN_NOTNULL void *dst);

/// @brief get the last element from the vector
///
/// @param vec the vector to get the value from
/// @param dst the destination to copy the value to
void typevec_tail(IN_NOTNULL const typevec_t *vec, IN_NOTNULL void *dst);

/// @brief push a value onto the vector
///
/// @param vec the vector to push the value onto
/// @param src the value to push
void *typevec_push(IN_NOTNULL typevec_t *vec, IN_NOTNULL const void *src);

/// @brief append multiple values onto the vector
/// @note this copies @p len * @a type_size bytes from @p src to the vector
///
/// @param vec the vector to append the values onto
/// @param src the values to append
/// @param len the number of values to append
void typevec_append(IN_NOTNULL typevec_t *vec, IN_NOTNULL const void *src, size_t len);

/// @brief pop a value from the vector
///
/// @param vec the vector to pop the value from
/// @param dst the destination to copy the value to
void typevec_pop(IN_NOTNULL typevec_t *vec, IN_NOTNULL void *dst);

/// @brief get a pointer to the value at the given index
///
/// @note the pointer is only valid until the next call to @a typevec_push or @a typevec_pop
///
/// @param vec the vector to get the value from
/// @param index the index to get the value from
/// @return void* a pointer to the value
NODISCARD
void *typevec_offset(IN_NOTNULL const typevec_t *vec, size_t index);

/// @brief get a pointer to the underlying data
///
/// @note the pointer is only valid until the next call to @a typevec_push or @a typevec_pop
///
/// @param vec the vector to get the data from
/// @return void* a pointer to the data
NODISCARD
void *typevec_data(IN_NOTNULL const typevec_t *vec);

/// @brief sort a vector
///
/// @param vec the vector to sort
/// @param cmp the comparison function
void typevec_sort(IN_NOTNULL typevec_t *vec, int (*cmp)(const void *, const void *));

void typevec_reset(IN_NOTNULL typevec_t *vec);

/// @}

END_API
