#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "log.h"
#include "alloc.h"
#include "output.h"

Output_module_t *OutputModules;


/*
 * run all output in output-modules
 */
void do_output(Config_t *config)
{
	Output_module_t *idx;

	idx = OutputModules;
	while (idx) {
		idx->output(config, idx->context);
	}
}

void register_output_module(
	const char *output_name, Output_func_t *output, void *context)
{
	Output_module_t *idx;

	idx = OutputModules;
	if (!idx) {
		OutputModules = alloc1(Output_module_t);
		OutputModules->output_name = strdup(output_name);
		OutputModules->output = output;
		OutputModules->context = context;
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
		idx->output = output;
		idx->context = context;
	}
}




extern void init_xml_output_module();
extern void init_socket_output_module();
extern void init_syslog_output_module();

void init_output_modules()
{
	init_xml_output_module();
	init_socket_output_module();
	init_syslog_output_module();
}

