#ifndef _PARSER_
#define _PARSER_

#include <stdint.h>
#include "types.h"
#include "format.h"
#include "version.h"
#include "osdep/osdep.h"
#include "resource.h"
#include <stdbool.h>


typedef struct Parser_operations_t {
	void (*init_parser)(char *);
	Config_t* (*do_parser)(char *);
	void (*finish_parser)(void);
} Parser_operations_t;

typedef struct Parser_module_t {
	bool enable;
	char *parser_name;
	Parser_operations_t op;
	struct Parser_module_t *next;
} Parser_module_t;


void init_parser_modules(char **module_names, int module_count);
Config_t *do_parser(const char *parser_name, char *args);
void finish_parser_modules(void);
void register_parser_module(const char *, Parser_operations_t *);
void setup_parser_modules(void);
void free_parser_modules(Parser_module_t *mod);

#endif
