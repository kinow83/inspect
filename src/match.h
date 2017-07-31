#ifndef _MATCH_MODULES_
#define _MATCH_MODULES_

#include "parser.h"

typedef Config_t (*Match_func_t)(Config_t *, uint8_t *, size_t, struct rx_info *);

typedef struct Match_module_t {
	char *match_name;
	Match_func_t match;
	struct Match_module_t *next;
} Match_module_t;

Config_t *do_match(Config_t *, uint8_t *, size_t, struct rx_info *);
void register_match_module(const char *, Match_func_t);
void init_match_modules();

#endif
