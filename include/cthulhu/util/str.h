#pragma once

#include <stdarg.h>

#include "macros.h"
#include "util.h"
#include "vector.h"

/**
 * @defgroup StringUtils String utility functions
 * @brief string manipulation and processing functions
 * @{
 */

/**
 * @brief format a string
 *
 * format a string with printf-like syntax
 *
 * @param fmt the format string
 * @param ... the arguments to format
 *
 * @return the formatted string
 */
char *format(const char *fmt, ...);

/**
 * @brief format a string with a @a va_list
 *
 * format a string with printf-like syntax with a va_list
 *
 * @param fmt the format string
 * @param args the va_list
 *
 * @return the formatted string
 */
char *formatv(const char *fmt, va_list args);

/**
 * @brief see if a string starts with a prefix
 *
 * check if a string starts with a substring
 *
 * @param str the string to search
 * @param prefix the prefix to check for
 *
 * @return if str starts with prefix
 */
bool str_startswith(const char *str, const char *prefix);

/**
 * check if a string ends with a substring
 *
 * @param str the string to search
 * @param suffix the suffix to check for
 *
 * @return if str ends with suffix
 */
bool str_endswith(const char *str, const char *suffix);

/**
 * @brief join strings
 *
 * join a vector of strings together with a separator
 *
 * @param sep the separator to use
 * @param parts a vector of strings to join
 *
 * @return the joined string
 */
char *str_join(const char *sep, vector_t *parts);

/**
 * @brief repeat a string
 *
 * repeat a string n times
 *
 * @param str the string to repeat
 * @param times the number of times to repeat
 *
 * @return the repeated string
 */
char *str_repeat(const char *str, size_t times);

/**
 * @brief turn a string into a C string literal
 *
 * normalize a string into a valid C string
 *
 * @param str the string to normalize
 *
 * @return the normalized string
 */
char *str_normalize(const char *str);

/**
 * @brief turn a string with length into a C string literal
 *
 * normalize a string with length into a valid C string
 *
 * @param str the string to normalize
 * @param len the length of the string
 *
 * @return the normalized string
 */
char *str_normalizen(const char *str, size_t len);

/**
 * @brief split a string into a vector by a separator
 *
 * @note the seperator is not included in the resulting substrings.
 * @note if no separator is found, the entire string is returned in the vectors
 * first element.
 *
 * @param str the string to split
 * @param sep the separator to split by
 *
 * @return the substrings
 */
vector_t *str_split(const char *str, const char *sep);

/**
 * @brief find the longest common prefix of a vector of paths
 *
 * @note if no common prefix is found, the empty string is returned.
 *
 * @param args the vector of paths to find the common prefix of
 *
 * @return the common prefix
 */
const char *common_prefix(vector_t *args);

size_t str_find(const char *str, const char *sub);

/**
 * @brief find the last instance of a substring in a string
 *
 * @param str the string to search
 * @param sub the substring to search for
 *
 * @return the index of the last instance of @a sub in @a str, or SIZE_MAX if
 * sub is not found
 */
size_t str_rfind(const char *str, const char *sub);

/**
 * @brief check if a string contains a substring
 *
 * @param str the string to search
 * @param sub the substring to search for
 *
 * @return if @a sub is found in @a str
 */
bool str_contains(const char *str, const char *sub);

/**
 * @brief replace all instances of a substring in a string
 *
 * @param str the string to replace elements in
 * @param sub the substring to replace
 * @param repl the replacement substring
 *
 * @return a copy of @a str with all instances of @a sub replaced with @a repl
 */
char *str_replace(const char *str, const char *sub, const char *repl);

char *str_trim(const char *str, const char *chars);

/**
 * @brief hash a string
 *
 * @param str the string to hash
 *
 * @return the hash
 */
size_t strhash(const char *str);

/**
 * @brief compare strings equality
 *
 * check if 2 strings are equal
 *
 * @param lhs the left hand side of the comparison
 * @param rhs the right hand side of the comparison
 *
 * @return if the strings are equal
 */
bool str_equal(const char *lhs, const char *rhs);

char *str_filename(const char *path);
char *str_noext(const char *path);

/** @} */
