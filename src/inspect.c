/*
 * inspect.c
 *
 *  Created on: 2017. 7. 31.
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include "output.h"
#include "match.h"
#include "parser.h"

void init_inspect_modules()
{
	init_parser_modules();
	init_match_modules();
	init_output_modules();
}


int main(int argc, char **argv)
{

	init_inspect_modules();

}
