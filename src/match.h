#ifndef _MATCH_MODULES_
#define _MATCH_MODULES_

#include "parser.h"
#include "osdep/osdep.h"

typedef struct Match_operations_t {
	void (*init_match)(void *);
	Action_t* (*do_match)(Config_t *, u8 *, size_t, struct rx_info *);
	void (*done_match)(void *);
} Match_operations_t;

typedef struct Match_module_t {
	char *match_name;
	Match_operations_t op;
	struct Match_module_t *next;
} Match_module_t;


void init_match(void *resource);
Action_t *do_match(Config_t *, u8 *, size_t, struct rx_info *);
void done_match(void *resource);
void register_match_module(const char *, Match_operations_t *);
void setup_match_modules(void);

#endif
