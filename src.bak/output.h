#ifndef _OUTPUT_MODULES_
#define _OUTPUT_MODULES_

#include "parser.h"
#include "osdep/osdep.h"
#include <stdbool.h>


typedef struct Output_wifi_data_t {
	u8 *h80211;
	size_t h80211len;
	struct rx_info *ri;
	struct timeval tv;
} Output_wifi_data_t;

typedef struct Output_operations_t {
	void (*init_output)(char *);
	void (*do_output)(Action_details_t *, void *);
	void (*finish_output)(void);
	const char* (*usage_output)(void);
} Output_operations_t;

typedef struct Output_module_t {
	bool enable;
	bool finished;
	char *output_name;
	Output_operations_t op;
	void *context;
	struct Output_module_t *next;
} Output_module_t;


void init_output_modules(Module_option_t *);
void do_output(Action_details_t *, void *);
void do_output_by_name(Action_details_t *, const char *, void *);
void finish_output_modules(void);
void register_output_module(const char *, Output_operations_t *);
void setup_output_modules(void);
void free_output_modules(Output_module_t *);
void usage_output_module(void);
int num_output_modules(void);
int num_enabled_output_modules(void);

#endif
