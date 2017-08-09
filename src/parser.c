#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "log.h"
#include "alloc.h"
#include "resource.h"
#include "parser.h"


static Parser_module_t *ParserModules;


#if 0
Parser_module_t *get_parsermodules(void)
{
	return ParserModules;
}
#endif

void init_parser_modules(Module_option_t *mopt)
{
	Parser_module_t *idx;
	int i = 0;

	if (!mopt) return;

	while (mopt) {
		idx = ParserModules;

		while (idx) {
			if (!strcasecmp(mopt->name, idx->parser_name)) {
				idx->op.init_parser(mopt->options);
				idx->enable = true;
				echo.d("enable module: %s", mopt->name);
				break;
			}
			idx = idx->next;
		}
		mopt = mopt->next;
	}
}

void finish_parser_modules(void)
{
	Parser_module_t *idx;

	idx = ParserModules;
	while (idx) {
		idx->op.finish_parser();
		idx = idx->next;
	}
}

/*
 * free pareser modules.
 */
void free_parser_modules(Parser_module_t *mod)
{
	Parser_module_t *idx;

	if (!mod) {
		mod = ParserModules;
	}

	while (mod) {
		idx = mod->next;
		if (mod->parser_name) free(mod->parser_name);
		free(mod);

		mod = idx;
	}
}

/*
 * load config by module name.
 */
Config_t *do_parser_modules(const char *parser_name)
{
	Parser_module_t *idx;
	Parser_operations_t *op;
	Config_t *config;
	Action_t *action;

	idx = ParserModules;
	while (idx) {
		if (!strcasecmp(idx->parser_name, parser_name) && idx->enable == true) {
			op = &idx->op;
			if (!op->do_parser) {
				return NULL;
			}
			config = op->do_parser();
			if (!config) {
				return NULL;
			}
			return config;
		}
		idx = idx->next;
	}
	return NULL;
}

void register_parser_module(const char *parser_name, Parser_operations_t *op)
{
	Parser_module_t *idx;

	idx = ParserModules;
	if (!idx) {
		ParserModules = alloc_sizeof(Parser_module_t);
		ParserModules->enable = false;
		ParserModules->parser_name = strdup(parser_name);
		ParserModules->op.init_parser = op->init_parser;
		ParserModules->op.do_parser = op->do_parser;
		ParserModules->op.finish_parser = op->finish_parser;
		ParserModules->op.usage_parser = op->usage_parser;
	}
	else {
		while (idx->next) {
			if (!strcasecmp(idx->parser_name, parser_name)) {
				echo.f("Duplicate parser module. %s", parser_name);
			}
			idx = idx->next;
		}

		idx->next = alloc_sizeof(Parser_module_t);

		idx = idx->next;

		idx->enable = false;
		idx->parser_name = strdup(parser_name);
		idx->op.init_parser = op->init_parser;
		idx->op.do_parser = op->do_parser;
		idx->op.finish_parser = op->finish_parser;
		idx->op.usage_parser = op->usage_parser;
	}
	echo.d("register parser module [%s]", parser_name);
}

void usage_parser_module(void)
{
	Parser_module_t *idx;

	idx = ParserModules;
	while (idx) {
		echo.out("\t\t[%s]", idx->parser_name);
		echo.out("\t\t\t%s", idx->op.usage_parser());
		idx = idx->next;
	}
}

int num_parser_modules(void)
{
	Parser_module_t *idx;
	int count = 0;

	idx = ParserModules;
	while (idx) {
		count++;
		idx = idx->next;
	}
	return count;
}

int num_enabled_parser_modules(void)
{
	Parser_module_t *idx;
	int count = 0;

	idx = ParserModules;
	while (idx) {
		if (idx->enable) {
			count++;
		}
		idx = idx->next;
	}
	return count;
}


extern void setup_xml_parser_module(void);

void setup_parser_modules()
{
	setup_xml_parser_module();
//	init_mysql_config();
//	init_sqlite_config();
}

