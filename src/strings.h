/*
 * strings.h
 *
 *  Created on: 2017. 8. 2.
 *      Author: root
 */

#ifndef SRC_STRINGS_H_
#define SRC_STRINGS_H_

#include <stdbool.h>


bool is_number(const char *str);
char **new_splits(const char *str, char *sep, int *ntoks);
void free_splits(char **toks, int ntoks);

#endif /* SRC_STRINGS_H_ */
