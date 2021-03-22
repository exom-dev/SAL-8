#ifndef SAL8_LIB_CHARLIB_H_GUARD
#define SAL8_LIB_CHARLIB_H_GUARD

#include "../api.h"

#include <stddef.h>
#include <stdbool.h>

SAL8_API_HIDDEN bool sal8_lib_charlib_is_digit(char c);
SAL8_API_HIDDEN bool sal8_lib_charlib_is_alphascore(char c);
SAL8_API_HIDDEN bool sal8_lib_charlib_strnicmp(const char* left, const char* right, size_t size);
SAL8_API_HIDDEN bool sal8_lib_charlib_strmcmp(const char* left, const char* right, size_t size); // Like strcmp, but right is not null-terminated, while left is. Size = right size.

SAL8_API_HIDDEN char* sal8_lib_charlib_strdup(const char* src);
SAL8_API_HIDDEN char* sal8_lib_charlib_strndup(const char* src, size_t size);

#endif
