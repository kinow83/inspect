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

int do_exit;


static void init_inspect_modules()
{
	setup_parser_modules();
	setup_match_modules();
	setup_output_modules();
}

static void* do_shooter(void *arg)
{

}

static void* do_capture(void *arg)
{

}

static void run_shooter(bool thread)
{
	pthread_t pid;
	if (thread) {
		if (pthread_create(&pid, NULL, (void*)do_shooter, &capture_g) != 0) {

	}
}

static void run_capture(bool thread)
{
	if (thread) {

	}
}

void usage(int argc, char **argv)
{
	echo.e("");
	echo.e("Usage: inspect <options>");
	echo.e("options:");
	echo.e("\t -c <config file>             : inspect config file");
	echo.e("\t -i <shooter-nic:capture-nic> : shooter and capture wifi interface");
	echo.e("\t -v                           : version");
	echo.e("\t -h                           : help");
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

}

int main(int argc, char **argv)
{
	int opt;

	while ((opt = getopt(argc, argv, "c:i:vh?")) != -1) {
		switch (opt) {
		case 'c':
			break;
		case 'i':
			break;
		case 'v':
			break;
		case 'h':
			break;
		case '?':
			break;
		default:
			break;
		}
	}


	init_inspect_modules();

}
