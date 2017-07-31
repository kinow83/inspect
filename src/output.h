#ifndef _OUTPUT_MODULES_
#define _OUTPUT_MODULES_

#include "parser.h"
#include "types.h"

typedef void (*Output_func_t)(Config_t *, u8 *);

typedef struct Output_module_t {
	char *output_name;
	Output_func_t output;
	void *context;
	struct Output_module_t *next;
} Output_module_t;

void do_output(Config_t *config, void *context);
void register_output_module(const char *, Output_func_t, void *);
void init_output_modules(void *context);

#endif
