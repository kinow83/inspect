/*
 * module.c
 *
 *  Created on: 2017. 8. 3.
 *      Author: root
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "module.h"
#include "alloc.h"



Module_do_chain_t *new_module_do_chain(const Module_do_chain_t *mod)
{
	Module_do_chain_t *head;

	head = alloc_sizeof(Module_do_chain_t);
	head->match = mod->match;
	head->output = mod->output;
	head->rtx = mod->rtx;
	head->next = NULL;

	return head;
}

Module_do_chain_t *add_module_do_chain(Module_do_chain_t *head, Module_do_chain_t *add)
{
	Module_do_chain_t *idx;

	if (!head) {
		return new_module_do_chain(add);
	}

	idx = head;
	while (!idx->next) {
		idx = idx->next;
	}

	idx->next = new_module_do_chain(add);

	return head;
}

void free_module_do_chain(Module_do_chain_t *head)
{
	Module_do_chain_t *idx, *tmp;

	idx = head;
	while (idx) {
		tmp = idx->next;
		free(tmp);
		idx = tmp;
	}
}
