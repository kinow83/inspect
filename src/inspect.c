/*
 * inspect.c
 *
 *  Created on: 2017. 7. 31.
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <getopt.h>
#include <libgen.h>
#include <stdbool.h>
#include "output.h"
#include "match.h"
#include "parser.h"
#include "osdep/osdep.h"
#include "resource.h"
#include "log.h"

int do_exit;


static void setup_modules(void)
{
	setup_parser_modules();
	setup_match_modules();
	setup_output_modules();
}

static void init_modules(Module_option_list_t *opts)
{
	init_parser_modules(opts->parser);
	init_match_modules(opts->match);
	init_output_modules(opts->output);
}

static void free_module_option_list(Module_option_list_t *opts)
{
	free_module_option(opts->parser);
	free_module_option(opts->match);
	free_module_option(opts->output);
}

static void finish_modules()
{
	finish_parser_modules();
	finish_match_modules();
	finish_output_modules();
}

static void free_modules()
{
	free_parser_modules(NULL);
	free_match_moduels(NULL);
	free_output_modules(NULL);
}

static void* do_shooter(void *arg)
{

}

static void* do_capture(void *arg)
{

}

static pthread_t run_or_thread(Config_t *config, bool thread, void *(*fp)(void *))
{
	pthread_t pid = 0;

	if (thread) {
		if (pthread_create(&pid, NULL, fp, config) != 0) {
			echo.f("Error pthread_create(): %s", strerror(errno));
		}
	}
	else {
		fp(config);
	}
	return pid;
}

void usage(int argc, char **argv)
{
	echo.e("");
	echo.e("Usage: inspect <options>");
	echo.e("options:");
	echo.e("\t -i <shooter NIC:capture NIC>    : shooter and capture wifi interface");
	echo.e("\t -o <output module name:options> : output module");
	echo.e("\t -p <parser module name:options> : parser module");
	echo.e("\t -m <match  module name:options> : match module");
	echo.e("\t -v                              : version");
	echo.e("\t -h -?                           : help");
	exit(1);

/*

 -o xml:filename=date.log,rotate=daily;sock:

 */
}

void sighandler(int signum)
{
	switch (signum) {
	case SIGUSR1:
		break;
	case SIGUSR2:
		break;
	case SIGINT:
		break;
	case SIGTERM:
		break;
	case SIGCHLD:
		break;
	default:
		break;
	}
}

int main(int argc, char **argv)
{
	int opt;
	char *interface_opts;
	Module_option_list_t mopt_list = {
			.output = NULL,
			.parser = NULL,
			.match  = NULL,
	};


	while ((opt = getopt(argc, argv, "i:o:p:m:vh?")) != -1) {
		switch (opt) {
		case 'i':
			interface_opts = strdup(optarg);
			break;
		case 'o':
			mopt_list.output = new_module_option(optarg);
			break;
		case 'p':
			mopt_list.parser = new_module_option(optarg);
			break;
		case 'm':
			mopt_list.match = new_module_option(optarg);
			break;
		case 'v':
			echo.i("version: %d", CUR_VERSION);
			break;
		case 'h':
		case '?':
			usage(argc, argv);
			break;
		default:
			usage(argc, argv);
			break;
		}
	}


	setup_modules();

	init_modules(&mopt_list);




done:
	finish_modules();
	free_modules();
	free_module_option_list(&mopt_list);
}
