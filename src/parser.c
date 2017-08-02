#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "alloc.h"
#include "resource.h"

static Parser_module_t *ParserModules;


void init_parser_modules(Module_option_t **parse_opts)
{
	Parser_module_t *idx;
	Module_option_t *opt;
	int i = 0;

	if (!parse_opts) return;

	while (parse_opts[i]) {
		opt = parse_opts[i];
		idx = ParserModules;

		while (idx) {
			if (!strcasecmp(opt->name, idx->parser_name)) {
				idx->op.init_parser(opt->options);
				idx->enable = true;
				break;
			}
			idx = idx->next;
		}
		i++;
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
 * free config modules.
 */
void free_parser_modules(Parser_module_t *mod)
{
	Parser_module_t *idx;

	if (!mod) {
		mod = ParserModules;
	}

	while (mod) {
		idx = mod->next;
		if (mod->parser_name) {
			free(mod->parser_name);
		}
		free(mod);

		mod = idx;
	}
}

/*
 * load config by module name.
 */
Config_t *do_parser(const char *config_name, char *args)
{
	Parser_module_t *idx;
	Parser_operations_t *op;
	Config_t *config;
	Action_t *action;

	idx = ParserModules;
	while (idx) {
		if (!strcasecmp(idx->parser_name, config_name)) {
			op = &idx->op;
			if (!op->do_parser) {
				return NULL;
			}
			config = op->do_parser(args);
			if (!config) {
				return NULL;
			}

			// sort action by 'no'
			sort_actions(&config->action);

			// allocate action's config to this config.
			action = config->action;
			while (action) {
				action->config = config;
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
	}
	else {
		while (idx->next) {
			if (!strcasecmp(idx->parser_name, parser_name)) {
				echo.f("Duplicate config module. %s", parser_name);
			}
			idx = idx->next;
		}

		idx->next = alloc_sizeof(Parser_module_t);
		idx->enable = false;
		idx->parser_name = strdup(parser_name);
		idx->op.init_parser = op->init_parser;
		idx->op.do_parser = op->do_parser;
		idx->op.finish_parser = op->finish_parser;
	}
	echo.d("register parser module [%s]", parser_name);
}



extern void setup_xml_parser_module(void);

void setup_parser_modules()
{
	setup_xml_parser_module();
//	init_mysql_config();
//	init_sqlite_config();
}

