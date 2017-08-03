/*
 * rtx.h
 *
 *  Created on: 2017. 8. 3.
 *      Author: root
 */

#ifndef SRC_RTX_H_
#define SRC_RTX_H_

#include <stdbool.h>
#include "resource.h"

typedef struct RTX_operations_t {
	void (*init_rtx)(char *);
	void (*do_rtx)(Config_t *);
	void (*finish_rtx)(void);
	const char* (*usage_rtx)(void);
} RTX_operations_t;

typedef struct RTX_module_t {
	bool enable;
	char *rtx_name;
	RTX_operations_t op;
	struct RTX_module_t *next;
} RTX_module_t;


void init_rtx_modules(Module_option_t *);
void finish_rtx_modules(void);
void free_rtx_modules(RTX_module_t *);
void do_rtx_modules(Config_t *);
void register_rtx_module(const char *, RTX_operations_t *);
void setup_rtx_modules(void);
void usage_rts_module(void);


#endif /* SRC_RTX_H_ */
