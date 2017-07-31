#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "log.h"
#include "alloc.h"
#include "output.h"

Output_module_t *OutputModules;


void init_output(void)
{
	Output_module_t *idx;

	idx = OutputModules;
	while (idx) {
		idx->op.init_output();
	}
}

void done_output(void)
{
	Output_module_t *idx;

	idx = OutputModules;
	while (idx) {
		idx->op.done_output();
	}
}

/*
 * run all output in output-modules
 */
void do_output(Action_t *action, u8 *output_data)
{
	Output_module_t *idx;

	idx = OutputModules;
	while (idx) {
		idx->op.do_output(action, output_data);
	}
}

void register_output_module(const char *output_name, Output_operations_t *op)
{
	Output_module_t *idx;

	idx = OutputModules;
	if (!idx) {
		OutputModules = alloc1(Output_module_t);
		OutputModules->output_name = strdup(output_name);
		OutputModules->op.init_output = op->init_output;
		OutputModules->op.do_output = op->do_output;
		OutputModules->op.done_output = op->done_output;
	}
	else {
		while (idx->next) {
			if (!strcasecmp(idx->output_name, output_name)) {
				echo.f("Duplicate output module. %s", output_name);
			}
			idx = idx->next;
		}

		idx->next = alloc1(Output_module_t);
		idx->output_name = strdup(output_name);
		idx->op.init_output = op->init_output;
		idx->op.do_output = op->do_output;
		idx->op.done_output = op->done_output;
	}
}




extern void init_xml_output_module(void);
//extern void init_socket_output_module();
//extern void init_syslog_output_module();

void init_output_modules(void)
{
	init_xml_output_module();
//	init_socket_output_module();
//	init_syslog_output_module();
}

