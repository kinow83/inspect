#ifndef _MATCH_MODULES_
#define _MATCH_MODULES_

#include "parser.h"
#include "osdep/osdep.h"
#include <stdbool.h>

typedef struct Match_operations_t {
	void (*init_match)(char *);
	Action_details_t* (*do_match)(Config_t *, u8 *, size_t, struct rx_info *);
	void (*finish_match)(void);
	const char* (*usage_match)(void);
} Match_operations_t;

typedef struct Match_module_t {
	bool enable;
	bool finished;
	char *match_name;
	Match_operations_t op;
	struct Match_module_t *next;
} Match_module_t;


void init_match_modules(Module_option_t *);
Action_details_t *do_match_modules(Config_t *, u8 *, size_t, struct rx_info *);
Action_details_t *do_match_by_name(Config_t *, const char *, u8 *, size_t, struct rx_info *);
void finish_match_modules(void);
void register_match_module(const char *, Match_operations_t *);
void setup_match_modules(void);
void free_match_moduels(Match_module_t *);
void usage_match_module(void);
int num_match_modules(void);
int num_enabled_match_modules(void);

#endif
