#ifndef _OUTPUT_MODULES_
#define _OUTPUT_MODULES_

#include "parser.h"

typedef struct Output_operations_t {
	void (*init_output)(void);
	void (*do_output)(Action_t *, u8 *);
	void (*done_output)(void);
} Output_operations_t;

typedef struct Output_module_t {
	char *output_name;
	Output_operations_t op;
	void *context;
	struct Output_module_t *next;
} Output_module_t;

void init_output(void);
void do_output(Action_t *, u8 *);
void done_output(void);

void register_output_module(const char *, Output_operations_t *);
void init_output_modules(void *context);

#endif
