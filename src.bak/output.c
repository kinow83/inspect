#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "log.h"
#include "alloc.h"
#include "output.h"
#include "string_util.h"

static Output_module_t *OutputModules;


#if 0
Output_module_t *get_output_modules(void)
{
	return OutputModules;
}
#endif

void init_output_modules(Module_option_t *mopt)
{
	Output_module_t *idx;
	int i = 0;

	if (!mopt) return;

	while (mopt) {
		idx = OutputModules;

		while (idx) {
			if (!strcasecmp(mopt->name, idx->output_name)) {
				idx->op.init_output(mopt->options);
				idx->enable = true;
				echo.d("enable module: %s", mopt->name);
				break;
			}
			idx = idx->next;
		}
		mopt = mopt->next;
	}
}

void finish_output_modules(void)
{
	u32 toolongtime = 0;
	Output_module_t *idx;

	idx = OutputModules;
	while (idx) {
		while (idx->finished == false) {
			usleep(10);
			toolongtime++;
			if (toolongtime > 10000) {
				echo.E("waiting too long time for finish output modules: %s", idx->output_name);
				toolongtime = 0;
			}
		}

		idx->op.finish_output();
		idx = idx->next;
	}
}

/*
 * run all output in output-modules
 */
void do_output(Action_details_t *detail, void *data)
{
	Output_module_t *idx;

	idx = OutputModules;
	while (idx) {
		if(idx->enable == true) {
			idx->finished = false;
			idx->op.do_output(detail, data);
			idx->finished = true;
		}
		idx = idx->next;
	}
}

void do_output_by_name(Action_details_t *detail, const char *name, void *data)
{
	Output_module_t *idx;

	idx = OutputModules;
	while (idx) {
		if((idx->enable == true) && !(strcasecmp(name, idx->output_name))) {
			idx->finished = false;
			idx->op.do_output(detail, data);
			idx->finished = true;
			break;
		}
		idx = idx->next;
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
		if (mod->output_name) free(mod->output_name);
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
		OutputModules->finished = true;
		OutputModules->output_name = strdup(output_name);
		OutputModules->op.init_output = op->init_output;
		OutputModules->op.do_output = op->do_output;
		OutputModules->op.finish_output = op->finish_output;
		OutputModules->op.usage_output = op->usage_output;
	}
	else {
		while (idx->next) {
			if (!strcasecmp(idx->output_name, output_name)) {
				echo.f("Duplicate output module. %s", output_name);
			}
			idx = idx->next;
		}

		idx->next = alloc_sizeof(Output_module_t);

		idx = idx->next;

		idx->enable = false;
		idx->finished = true;
		idx->output_name = strdup(output_name);
		idx->op.init_output = op->init_output;
		idx->op.do_output = op->do_output;
		idx->op.finish_output = op->finish_output;
		idx->op.usage_output = op->usage_output;
	}
	echo.d("register output module [%s]", output_name);
}

void usage_output_module(void)
{
	Output_module_t *idx;

	idx = OutputModules;
	while (idx) {
		echo.out("\t\t[%s]", idx->output_name);
		echo.out("\t\t\t%s", idx->op.usage_output());
		idx = idx->next;
	}
}

int num_output_modules(void)
{
	Output_module_t *idx;
	int count = 0;

	idx = OutputModules;
	while (idx) {
		count++;
		idx = idx->next;
	}
	return count;
}

int num_enabled_output_modules(void)
{
	Output_module_t *idx;
	int count = 0;

	idx = OutputModules;
	while (idx) {
		if (idx->enable) {
			count++;
		}
		idx = idx->next;
	}
	return count;
}


extern void setup_wifi_xml_output_module(void);
extern void setup_error_output_module(void);

void setup_output_modules(void)
{
	setup_wifi_xml_output_module();
	setup_error_output_module();
}

