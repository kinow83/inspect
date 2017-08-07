/*
 * error_output.c
 *
 *  Created on: 2017. 8. 4.
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "format.h"
#include "log.h"
#include "alloc.h"
#include "output.h"
#include "strings.h"

#define ERROR_OUTPUT_NAME "error"

static FILE *ErrFp;
static char *ErrFilename;

static void do_error_output(Action_t *action, void *output_data)
{
	char *data = (char *)output_data;

	if (ErrFp) {
		fprintf(ErrFp, "%s\n", data);
	}

	mark_finished_output_module(ERROR_OUTPUT_NAME);
}

static void init_error_output(char *options)
{
	char **chunk, **field;
	int nchunk, nfield;
	int i;

	chunk = new_splits(options, ",", &nchunk);
	if (!chunk || nchunk == 0) {
		echo.f("error error_output: invalid option: %s", options);
	}
	for (i=0; i<nchunk; i++) {
		field = new_splits(chunk[i], "=", &nfield);
		if (!field || nfield != 2) {
			echo.f("error error_output: invalid option: %s", chunk[i]);
		}
		// output error filename
		if (!strcasecmp("filename", field[0])) {
			ErrFilename = strdup(field[1]);
			ErrFp = fopen(ErrFilename, "w");
			if (!ErrFp) {
				echo.f("error error_output: open '%s' file: %s", ErrFilename, strerror(errno));
			}
		}
		free_splits(field, nfield);
	}
	free_splits(chunk, nchunk);

	if (!ErrFp) {
		echo.f("error error_output: empty file: %s", options);
	}

	echo.i("[error output options]");
	echo.i("file = %s", ErrFilename);
}

static void finish_error_output(void)
{
	if (ErrFp) {
		fclose(ErrFp);
		ErrFp = NULL;
	}
	if (ErrFilename) {
		free(ErrFilename);
		ErrFilename = NULL;
	}
}

static const char *usage_error_output(void)
{
	return "error:filename=<error filename>";
}

void setup_error_output_module(void)
{
	Output_operations_t op = {
			.init_output   = init_error_output,
			.do_output     = do_error_output,
			.finish_output = finish_error_output,
			.usage_output  = usage_error_output,
	};

	register_output_module(ERROR_OUTPUT_NAME, &op);
}
