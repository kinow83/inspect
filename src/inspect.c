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
#include "rtx.h"
#include "osdep/osdep.h"
#include "resource.h"
#include "log.h"
#include "module.h"
#include "string_util.h"
#include "thread_wait.h"

bool do_exit;


static void setup_modules(void)
{
	setup_parser_modules();
	setup_match_modules();
	setup_output_modules();
	setup_rtx_modules();
}

static void init_modules(Module_option_list_t *opts)
{
	init_parser_modules(opts->parser);
	init_match_modules(opts->match);
	init_output_modules(opts->output);
	init_rtx_modules(opts->rtx);
}

static void free_module_option_list(Module_option_list_t *opts)
{
	free_module_option(opts->parser);
	free_module_option(opts->match);
	free_module_option(opts->output);
	free_module_option(opts->rtx);
}

static void finish_modules()
{
	finish_parser_modules();
	finish_match_modules();
	finish_output_modules();
	finish_rtx_modules();
}

static void free_modules()
{
	free_parser_modules(NULL);
	free_match_moduels(NULL);
	free_output_modules(NULL);
	free_rtx_modules(NULL);
}

void usage(int argc, char **argv)
{
	echo.out("");
	echo.OUT("Usage: inspect <options>");
	echo.out("options:");
	echo.out("\t -r <rtx module name:options;...>    : rtx module");
	usage_rtx_module();
	echo.out("\t -o <output module name:options;...> : output module");
	usage_output_module();
	echo.out("\t -p <parser module name:options;...> : parser module");
	usage_parser_module();
	echo.out("\t -m <match  module name:options;...> : match module");
	usage_match_module();
	echo.out("\t -v                              : version");
	echo.out("\t -h -?                           : help");
	exit(1);
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
	do_exit = true;
	echo.I("catch signal");
}

int main(int argc, char **argv)
{
	Config_t *config;
	int opt;
	Module_option_list_t mopt_list = {NULL,};

	setup_modules();

	while ((opt = getopt(argc, argv, "r:o:p:m:vh?")) != -1) {
		switch (opt) {
		case 'r':
			mopt_list.rtx = new_module_option(optarg);
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
			echo.out("version: %d", CUR_VERSION);
			debug_h80211_type_names();
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

	if (get_module_option_count(mopt_list.parser) != 1) {
		echo.OUT("missing option: -p");
		exit(1);
	}

	// initialize modules
	init_modules(&mopt_list);

	config = do_parser_modules(mopt_list.parser->name);
	if (!config) {
		echo.OUT("error parsing for %s", mopt_list.parser->name);
		exit(1);
	} else {
		debug_config(config);
	}

	do_exit = false;

    signal(SIGINT,  sighandler);
    signal(SIGTERM, sighandler);

	// run rtx modules
	do_rtx_modules(config);

	do_exit = true;


done:
#ifdef THREAD_WAIT_ABLE
	echo.d("[wait all thread]");
	thread_wait();
	free_thread_wait(NULL);
#endif

	echo.d("[free config]");
	free_config(config);

	echo.d("[finish modules]");
	finish_modules();

	echo.d("[free modules]");
	free_modules();

	echo.d("[free module option list]");
	free_module_option_list(&mopt_list);
}
