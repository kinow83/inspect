/*
 * cmdutil.h
 *
 *  Created on: 2017. 8. 9.
 *      Author: root
 */

#ifndef SRC_CMDUTIL_H_
#define SRC_CMDUTIL_H_

#include <stdbool.h>

typedef struct Cmd_util_t {
	void *ctx;
	char *name;
	void (*init)(void);
	bool (*run)(int, char **);
	void (*finish)(void);
	void (*usage)(int, char **);
} Cmd_util_t;

#endif /* SRC_CMDUTIL_H_ */
