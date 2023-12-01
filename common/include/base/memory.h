#pragma once

#include "base/analyze.h"

#include <stddef.h>
#include <stdint.h>

BEGIN_API

#define ALLOC_SIZE_UNKNOWN SIZE_MAX

/// @defgroup Memory Management
/// @brief global and arena memory management
/// @{

typedef struct alloc_t alloc_t;

/// @brief malloc function pointer
///
/// @param self associated allocator
/// @param size the size of the allocation
/// @param name the name of the allocation
///
/// @return the allocated pointer
/// @return NULL if the allocation failed
typedef void *(*malloc_t)(alloc_t *self, size_t size, const char *name);

/// @brief realloc function pointer
///
/// @param self associated allocator
/// @param ptr the pointer to reallocate
/// @param new_size the new size of the allocation
/// @param old_size the old size of the allocation
///
/// @return the reallocated pointer
/// @return NULL if the allocation failed
typedef void *(*realloc_t)(alloc_t *self, void *ptr, size_t new_size, size_t old_size);

/// @brief free function pointer
///
/// @param self associated allocator
/// @param ptr the pointer to free
/// @param size the size of the allocation
typedef void (*free_t)(alloc_t *self, void *ptr, size_t size);

/// @brief an allocator object
typedef struct alloc_t
{
    const char *name;        ///< the name of the allocator
    malloc_t arena_malloc;   ///< the malloc function
    realloc_t arena_realloc; ///< the realloc function
    free_t arena_free;       ///< the free function
} alloc_t;

/// @brief the default allocator
/// @note this is the default allocator used by @a ctu_malloc, @a ctu_realloc, and @a ctu_free
/// @note exercise caution swapping this allocator during a compiler run
extern alloc_t gDefaultAlloc;

/// @brief free a pointer allocated with ctu_malloc or ctu_realloc
/// @note @a gDefaultAlloc must be consistent with the allocator used to allocate @a ptr
///
/// @param ptr the pointer to free, must not be NULL
void ctu_free(IN_NOTNULL void *ptr);

/// @brief allocate a pointer from the default allocator
///
/// @param size the size of the allocation
///
/// @return the allocated pointer
NODISCARD ALLOC(ctu_free)
void *ctu_malloc(size_t size);

/// @brief reallocate a pointer from the default allocator
/// @note @a gDefaultAlloc must be consistent with the allocator used to allocate @a ptr
///
/// @param ptr the pointer to reallocate
/// @param new_size the new size of the allocation
///
/// @return the reallocated pointer
NODISCARD ALLOC(ctu_free)
void *ctu_realloc(IN_NOTNULL void *ptr, size_t new_size);

/// @brief initialize gmps allocator to use a custom allocator
///
/// @param alloc the allocator to use
void init_gmp(IN_NOTNULL alloc_t *alloc);

/// @brief release memory from a custom allocator
/// @note ensure the allocator is consistent with the allocator used to allocate @a ptr
///
/// @param alloc the allocator to use
/// @param ptr the pointer to free
/// @param size the size of the allocation
void arena_free(IN_NOTNULL alloc_t *alloc, IN_NOTNULL void *ptr, size_t size);

/// @brief allocate memory from a custom allocator
///
/// @param alloc the allocator to use
/// @param size the size of the allocation, must be greater than 0
/// @param name the name of the allocation
///
/// @return the allocated pointer
NODISCARD ALLOC(arena_free, 2)
void *arena_malloc(IN_NOTNULL alloc_t *alloc, size_t size, const char *name);

/// @brief resize a memory allocation from a custom allocator
/// @note ensure the allocator is consistent with the allocator used to allocate @a ptr
///
/// @param alloc the allocator to use
/// @param ptr the pointer to reallocate
/// @param new_size the new size of the allocation
/// @param old_size the old size of the allocation
///
/// @return the reallocated pointer
NODISCARD
void *arena_realloc(IN_NOTNULL alloc_t *alloc, IN_NOTNULL void *ptr, size_t new_size, size_t old_size);

/// @} // Memory Management

END_API
