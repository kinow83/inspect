/*
 * string_util.h
 *
 *  Created on: 2017. 8. 2.
 *      Author: root
 */

#ifndef SRC_STRING_UTIL_H_
#define SRC_STRING_UTIL_H_

#include <stdbool.h>

/*
 * function: str_str
 * when mismatched.
 */
#define MISS_STR_STR (((void *)0)-1)


char *str_str(const char *str, const char *needle);
bool is_number(const char *str);
char **new_splits(const char *str, char *sep, int *ntoks);
void free_splits(char **toks, int ntoks);

#endif /* SRC_STRING_UTIL_H_ */
