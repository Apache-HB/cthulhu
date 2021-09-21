#pragma once

#include <stddef.h>
#include <stdbool.h>

#ifdef __GNUC__
#   define PRINT(fmt, args) __attribute__((format(printf, fmt, args)))
#else
#   define PRINT(fmt, args)
#endif

#define MAX(L, R) ((L) > (R) ? (L) : (R)) 
#define MIN(L, R) ((L) < (R) ? (L) : (R)) 

#define UNUSED(x) ((void)(x))

// memory managment

void *ctu_malloc(size_t size);
void *ctu_realloc(void *ptr, size_t size);
void ctu_free(void *ptr);
char *ctu_strdup(const char *str);
void *ctu_memdup(const void *ptr, size_t size);
void init_memory(void);

#define NEW(type) ((type *)ctu_malloc(sizeof(type)))
#define NEW_ARRAY(type, count) ((type *)ctu_malloc(sizeof(type) * (count)))
#define DELETE(ptr) ctu_free(ptr)
#define DELETE_ARRAY(ptr, len) ctu_free(ptr)

// map collection

typedef struct entry_t {
    const char *key;
    void *value;
    struct entry_t *next;
} entry_t;

typedef struct {
    size_t size;
    entry_t data[];
} map_t;

typedef void(*map_apply_t)(void *user, void *value);
typedef bool(*map_collect_t)(void *value);
typedef void*(*vector_apply_t)(void *value);

/**
 * a map size enum.
 * 
 * a curious case with compilers is that maps either need to be massive
 * or miniscule. a toplevel module may have hundreds of functions, variables, imports etc.
 * then function bodies maybe have 10 variables including arguments.
 * 
 * symbol tables seemingly follow this rule of either exporting 1 or 2 functions
 * or exporting an entire library with thousands of symbols.
 *
 * we pick a few arbitrary primes
 */
typedef enum {
    MAP_SMALL = 7, /// optimal for maps with less than 10 items
    MAP_BIG = 97 /// optimal for maps with 100 or more items
} map_size_t;

/**
 * create a new map
 * 
 * @param size the number of toplevel buckets used.
 *             increasing this value reduces hash collisions
 *             but increases memory usage and initialization time.
 * 
 * @return a new map
 */
map_t *map_new(map_size_t size);

/**
 * delete a map. elements of the map are not freed
 * 
 * @param map the map to delete
 */
void map_delete(map_t *map);

/**
 * get a value from a map
 * 
 * @param map the map to get the value from
 * @param key the key to get the value for
 * 
 * @return the value for the key or NULL if the key is not found
 */
void *map_get(map_t *map, const char *key);

/**
 * set or overwrite a value in a map
 * 
 * @param map the map to set the value in
 * @param key the key to set the value for
 * @param value the value to set
 */
void map_set(map_t *map, const char *key, void *value);

/**
 * apply a function to all values in a map
 * 
 * @param map the map to apply the function to
 * @param user user data passed into func
 * @param func the function to apply
 */
void map_apply(map_t *map, void *user, 
               map_apply_t func);

// vector collection

typedef struct {
    size_t size;
    size_t used;
    void *data[];
} vector_t;

/**
 * create a new vector.
 * 
 * @param size the initial amount of allocated memory
 * @return a new vector
 */
vector_t *vector_new(size_t size);

/**
 * create a new vector with a specified size
 * 
 * @param len the initial size of the vector
 * @return a new vector
 */
vector_t *vector_of(size_t len);

/**
 * create a vector with a single item
 * 
 * @param value the initial element
 * @return the new vector
 */
vector_t *vector_init(void *value);

/**
 * release a vector
 * 
 * @param vector the vector to release
 */
void vector_delete(vector_t *vector);

/**
 * push an element onto the end of a vector
 * 
 * @param vector a pointer to a vector returned by vector_new.
 *               the pointer may be modified by this function.
 * 
 * @param value the value to push onto the vector
 */
void vector_push(vector_t **vector, void *value);

/**
 * pop the last element from a vector.
 * calling on an empty vector is invalid.
 * 
 * @param vector the vector to take from
 * @return the value of the last element
 */
void *vector_pop(vector_t *vector);

/**
 * set an element in a vector by index.
 * index must not be out of range.
 * 
 * @param vector the vector to set in
 * @param index the index to place the value
 * @param value the value to place
 */
void vector_set(vector_t *vector, size_t index, void *value);

/**
 * get an element from a vector by index.
 * index must not be out of range.
 * 
 * @param vector the vector to get from
 * @param index the index to query
 * @return the value at index
 */
void *vector_get(const vector_t *vector, size_t index);

/**
 * get the last element from a vector.
 * calling on an empty vector is invalid.
 * 
 * @param vector the vector to get from
 * @return the value of the last element
 */
void *vector_tail(const vector_t *vector);

/**
 * get the contents pointer of a vector.
 * 
 * @param vector the vector to get the contents of
 * @return the contents pointer
 */
void **vector_data(vector_t *vector);

/**
 * get the length of a vector
 * 
 * @param vector the vector to get the length of
 * @return the active size of the vector
 */
size_t vector_len(const vector_t *vector);

/**
 * join two vectors together into a new vector.
 * 
 * @param lhs the left vector
 * @param rhs the right vector
 * @return the new vector
 */
vector_t *vector_join(const vector_t *lhs, const vector_t *rhs);

/**
 * return a new vector after applying a function to all elements
 * 
 * @param vector the vector to map from
 * @param func the function to apply
 * @return the new vector
 */
vector_t *vector_map(const vector_t *vector, vector_apply_t func);

vector_t *map_collect(map_t *map, map_collect_t filter);

#define MAP_APPLY(map, user, func) map_apply(map, user, (map_apply_t)func)
#define MAP_COLLECT(map, filter) map_collect(map, (map_collect_t)filter)
#define VECTOR_MAP(vector, func) vector_map(vector, (vector_apply_t)func)
