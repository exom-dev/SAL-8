#ifndef SAL8_LIB_CHARLIB_H_GUARD
#define SAL8_LIB_CHARLIB_H_GUARD

#include <stddef.h>
#include <stdbool.h>

bool sal8_lib_charlib_is_digit(char c);
bool sal8_lib_charlib_is_alpha(char c);
bool sal8_lib_charlib_strnicmp(const char* left, const char* right, size_t size);
bool sal8_lib_charlib_strmcmp(const char* left, const char* right, size_t size); // Like strcmp, but right is not null-terminated, while left is. Size = right size.

char* sal8_lib_charlib_strdup(const char* src);
char* sal8_lib_charlib_strndup(const char* src, size_t size);

#endif
