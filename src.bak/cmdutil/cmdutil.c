/*
 * cmdutil.c
 *
 *  Created on: 2017. 8. 9.
 *      Author: root
 */


#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cmdutil.h"

extern Cmd_util_t iw;

static Cmd_util_t *Modules[] = {
		&iw,
		NULL
};


static void usage(int argc, char **argv)
{
	Cmd_util_t **module;
	int i;

	printf("Usage: cmdutil [<option>]...\n");

	for (i = 0, module = Modules;
			*module;
			module++) {
		if ((*module)->usage) {
			(*module)->usage(argc, argv);
		}
	}
}

static Cmd_util_t *find_module(const char *name)
{
	Cmd_util_t **module;
	int i;

	for (i = 0, module = Modules;
			*module;
			module++) {
		if (!strcmp(name, (*module)->name)) {
			return (*module);
		}
	}
	return NULL;
}

int main(int argc, char **argv)
{
	Cmd_util_t *module;
	bool ok;

	if (argc < 2) {
		usage(argc, argv);
		exit(1);
	}

	module = find_module(argv[1]);
	if (!module) {
		usage(argc, argv);
		exit(1);
	}

	if (module->init) {
		module->init();
	}

	ok = module->run(argc-2, &argv[2]);

	if (module->finish) {
		module->finish();
	}

	printf("\n");

	return ok ? 0 : -1;
}
