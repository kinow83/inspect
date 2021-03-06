/*
 * rtx.c
 *
 *  Created on: 2017. 8. 3.
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "log.h"
#include "alloc.h"
#include "rtx.h"

static RTX_module_t *RTXModules;


#if 0
RTX_module_t *get_rtx_modules(void)
{
	return RTXModules;
}
#endif

void init_rtx_modules(Module_option_t *mopt)
{
	RTX_module_t *idx;

	if (!mopt) return;

	while (mopt) {
		idx = RTXModules;

		while (idx) {
			if (!strcasecmp(mopt->name, idx->rtx_name)) {
				idx->op.init_rtx(mopt->options);
				idx->enable = true;
				echo.d("enable module: %s", mopt->name);
				break;
			}
			idx = idx->next;
		}
		mopt = mopt->next;
	}
}

void finish_rtx_modules(void)
{
	u32 toolongtime = 0;
	RTX_module_t *idx;

	idx = RTXModules;
	while (idx) {
		while (idx->finished == false) {
			usleep(10);
			toolongtime++;
			if (toolongtime > 10000) {
				echo.E("waiting too long time for finish rtx modules: %s", idx->rtx_name);
				toolongtime = 0;
			}
		}

		idx->op.finish_rtx();
		idx = idx->next;
	}
}

void free_rtx_modules(RTX_module_t *mod)
{
	RTX_module_t *idx;

	if (!mod) {
		mod = RTXModules;
	}
	while (mod) {
		idx = mod->next;
		if (mod->rtx_name) free(mod->rtx_name);
		free(mod);

		mod = idx;
	}
}

void do_rtx_modules(Config_t *config)
{
	RTX_module_t *idx;

	idx = RTXModules;
	while (idx) {
		if (idx->enable == true) {
			idx->finished = false;
			idx->op.do_rtx(config, idx);
			idx->finished = true;
		}
		idx = idx->next;
	}
}

void do_rtx_modules_by_name(Config_t *config, const char *name)
{
	RTX_module_t *idx;

	idx = RTXModules;
	while (idx) {
		if ((idx->enable == true) && !strcasecmp(name, idx->rtx_name)) {
			idx->finished = false;
			idx->op.do_rtx(config, idx);
			idx->finished = true;
			return;
		}
		idx = idx->next;
	}
}

void register_rtx_module(const char *rtx_name, RTX_operations_t *op)
{
	RTX_module_t *idx;

	idx = RTXModules;
	if (!idx) {
		RTXModules = alloc_sizeof(RTX_module_t);
		RTXModules->enable = false;
		RTXModules->finished = true;
		RTXModules->rtx_name = strdup(rtx_name);
		RTXModules->op.init_rtx = op->init_rtx;
		RTXModules->op.do_rtx = op->do_rtx;
		RTXModules->op.finish_rtx = op->finish_rtx;
		RTXModules->op.usage_rtx = op->usage_rtx;
	}
	else {
		while (idx->next) {
			if (!strcasecmp(idx->rtx_name, rtx_name)) {
				echo.f("Duplicate rtx module. %s", rtx_name);
			}
			idx = idx->next;
		}

		idx->next = alloc_sizeof(RTX_module_t);

		idx = idx->next;

		idx->enable = false;
		idx->finished = true;
		idx->rtx_name = strdup(rtx_name);
		idx->op.init_rtx = op->init_rtx;
		idx->op.do_rtx = op->do_rtx;
		idx->op.finish_rtx = op->finish_rtx;
		idx->op.usage_rtx = op->usage_rtx;
	}
	echo.d("register rtx module [%s]", rtx_name);
}

void usage_rtx_module(void)
{
	RTX_module_t *idx;

	idx = RTXModules;
	while (idx) {
		echo.out("\t\t[%s]", idx->rtx_name);
		echo.out("\t\t\t%s", idx->op.usage_rtx());
		idx = idx->next;
	}
}

int num_rtx_modules(void)
{
	RTX_module_t *idx;
	int count = 0;

	idx = RTXModules;
	while (idx) {
		count++;
		idx = idx->next;
	}
	return count;
}

int num_enabled_rtx_modules(void)
{
	RTX_module_t *idx;
	int count = 0;

	idx = RTXModules;
	while (idx) {
		if (idx->enable) {
			count++;
		}
		idx = idx->next;
	}
	return count;
}


extern void setup_wifi_rtx_module();

void setup_rtx_modules(void)
{
	setup_wifi_rtx_module();
}

