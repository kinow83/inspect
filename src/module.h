/*
 * module.h
 *
 *  Created on: 2017. 8. 3.
 *      Author: root
 */

#ifndef SRC_MODULE_H_
#define SRC_MODULE_H_

#include "output.h"
#include "parser.h"
#include "rtx.h"
#include "match.h"


typedef struct Module_do_chain_t {
	Parser_module_t *parser;
	RTX_module_t *rtx;
	Match_module_t *match;
	Output_module_t *output;
	struct Module_do_chain_t *next;
} Module_do_chain_t;

Module_do_chain_t *new_module_do_chain(const Module_do_chain_t *);
Module_do_chain_t *add_module_do_chain(Module_do_chain_t *, Module_do_chain_t *);
void free_module_do_chain(Module_do_chain_t *);


#endif /* SRC_MODULE_H_ */
