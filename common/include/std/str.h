#pragma once

#include "base/analyze.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

BEGIN_API

typedef struct vector_t vector_t;
typedef struct map_t map_t;

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
NODISCARD FORMAT_ATTRIB(1, 2)
char *format(FORMAT_STRING const char *fmt, ...);

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
NODISCARD
char *formatv(IN_STRING const char *fmt, va_list args);

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
NODISCARD CONSTFN 
bool str_startswith(IN_STRING const char *str, IN_STRING const char *prefix);

/**
 * check if a string ends with a substring
 *
 * @param str the string to search
 * @param suffix the suffix to check for
 *
 * @return if str ends with suffix
 */
NODISCARD CONSTFN 
bool str_endswith(IN_STRING const char *str, IN_STRING const char *suffix);

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
NODISCARD
char *str_join(IN_STRING const char *sep, IN_NOTNULL vector_t *parts);

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
NODISCARD
char *str_repeat(IN_STRING const char *str, size_t times);

/**
 * @brief turn a string into a C string literal
 *
 * normalize a string into a valid C string
 *
 * @param str the string to normalize
 *
 * @return the normalized string
 */
NODISCARD
char *str_normalize(IN_STRING const char *str);

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
NODISCARD
char *str_normalizen(IN_READS(len) const char *str, size_t len);

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
NODISCARD
vector_t *str_split(IN_STRING const char *str, IN_STRING const char *sep);

/**
 * @brief find the longest common prefix of a vector of paths
 *
 * @note if no common prefix is found, the empty string is returned.
 *
 * @param args the vector of paths to find the common prefix of
 *
 * @return the common prefix
 */
NODISCARD
const char *common_prefix(IN_NOTNULL vector_t *args);

/**
 * @brief find the last instance of a substring in a string
 *
 * @param str the string to search
 * @param sub the substring to search for
 *
 * @return the index of the last instance of @a sub in @a str, or SIZE_MAX if
 * sub is not found
 */
MUST_INSPECT CONSTFN 
size_t str_rfind(IN_STRING const char *str, IN_STRING const char *sub);

/**
 * @brief find the last instance of a substring in a string with provided length
 *
 * @param str the string to search
 * @param len the length of @a str
 * @param sub the substring to search for
 * @return the index of the last instance of @a sub in @a str, or SIZE_MAX
 */
MUST_INSPECT CONSTFN 
size_t str_rfindn(IN_READS(len) const char *str, size_t len, IN_STRING const char *sub);

/**
 * @brief check if a string contains a substring
 *
 * @param str the string to search
 * @param sub the substring to search for
 *
 * @return if @a sub is found in @a str
 */
NODISCARD CONSTFN 
bool str_contains(IN_STRING const char *str, IN_STRING const char *sub);

/**
 * @brief replace all instances of a substring in a string
 *
 * @param str the string to replace elements in
 * @param sub the substring to replace
 * @param repl the replacement substring
 *
 * @return a copy of @a str with all instances of @a sub replaced with @a repl
 */
NODISCARD
char *str_replace(IN_STRING const char *str, IN_STRING const char *sub, IN_STRING const char *repl);

NODISCARD
char *str_replace_many(IN_STRING const char *str, IN_NOTNULL map_t *repl);

/**
 * @brief trim leading and trailing characters from a string
 *
 * @param str the string to trim
 * @param letters the letters to be removed
 * @return the trimmed string
 */
NODISCARD
char *str_trim(IN_STRING const char *str, IN_STRING const char *letters);

/**
 * @brief remove all instances of @a letters from @a str
 *
 * @param str the string to erase letters from
 * @param len the length of @a str
 * @param letters the letters to erase
 * @return the string with letters removed
 */
NODISCARD
char *str_erase(IN_READS(len) const char *str, size_t len, IN_STRING const char *letters);

/**
 * @brief hash a string
 *
 * @param str the string to hash
 *
 * @return the hash
 */
NODISCARD CONSTFN 
size_t strhash(IN_STRING const char *str);

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
NODISCARD CONSTFN 
bool str_equal(IN_STRING const char *lhs, IN_STRING const char *rhs);

/**
 * @brief get the filename from @a path
 *
 * @param path the path to get the filename from
 * @return the filename extracted from @a path
 */
NODISCARD
char *str_filename(IN_STRING const char *path);

/**
 * @brief remove the last file extension from a path
 *
 * @param path the path to remove the extension from
 * @return the @a path with the last extension removed
 */
NODISCARD
char *str_noext(IN_STRING const char *path);

/**
 * @brief uppercase an ascii string
 *
 * @param str the string
 * @return @a str with all lowercase charaters replaced with uppercase
 */
NODISCARD
char *str_upper(IN_STRING const char *str);

NODISCARD
char *str_lower(IN_STRING const char *str);

NODISCARD CONSTFN 
char str_tolower(int c);

#define STR_WHITESPACE " \t\r\v\n\f" //< all whitespace charaters

/** @} */

END_API
