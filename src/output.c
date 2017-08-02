#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "log.h"
#include "alloc.h"
#include "output.h"

static Output_module_t *OutputModules;


void init_output_modules(Module_option_t **output_opts)
{
	Output_module_t *idx;
	Module_option_t *opt;
	int i = 0;

	if (!output_opts) return;

	while (output_opts[i]) {
		opt = output_opts[i];
		idx = OutputModules;

		while (idx) {
			if (!strcasecmp(opt->name, idx->output_name)) {
				idx->op.init_output(opt->options);
				idx->enable = true;
				break;
			}
			idx = idx->next;
		}
		i++;
	}
}

void finish_output_modules(void)
{
	Output_module_t *idx;

	idx = OutputModules;
	while (idx) {
		idx->op.finish_output();
	}
}

/*
 * run all output in output-modules
 */
void do_output(Action_t *action, Output_data_t *data)
{
	Output_module_t *idx;

	idx = OutputModules;
	while (idx) {
		idx->op.do_output(action, data);
	}
}

void free_output_modules(Output_module_t *mod)
{
	Output_module_t *idx;

	if (!mod) {
		mod = OutputModules;
	}
	while (mod) {
		idx = mod->next;
		if (mod->output_name) {
			free(mod->output_name);
		}
		free(mod);

		mod = idx;
	}
}

void register_output_module(const char *output_name, Output_operations_t *op)
{
	Output_module_t *idx;

	idx = OutputModules;
	if (!idx) {
		OutputModules = alloc_sizeof(Output_module_t);
		OutputModules->enable = false;
		OutputModules->output_name = strdup(output_name);
		OutputModules->op.init_output = op->init_output;
		OutputModules->op.do_output = op->do_output;
		OutputModules->op.finish_output = op->finish_output;
	}
	else {
		while (idx->next) {
			if (!strcasecmp(idx->output_name, output_name)) {
				echo.f("Duplicate output module. %s", output_name);
			}
			idx = idx->next;
		}

		idx->next = alloc_sizeof(Output_module_t);
		idx->enable = false;
		idx->output_name = strdup(output_name);
		idx->op.init_output = op->init_output;
		idx->op.do_output = op->do_output;
		idx->op.finish_output = op->finish_output;
	}
	echo.d("register output module [%s]", output_name);
}




extern void setup_xml_output_module(void);
//extern void setup_socket_output_module();
//extern void setup_syslog_output_module();

void setup_output_modules(void)
{
	setup_xml_output_module();
//	setup_socket_output_module();
//	setup_syslog_output_module();
}

