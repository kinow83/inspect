/*
 * test.c
 *
 *  Created on: 2016. 12. 7.
 *      Author: root
 */


#include <stdio.h>
#include <unistd.h>
#include "ezxml.h"

int main(int argc, char **argv)
{
	ezxml_t xml = ezxml_parse_file("test.xml");
	ezxml_t e;

	for (	e = ezxml_child(xml, "airconf");
			e;
			e = e->next)
	{
		const char *p;

		printf("%s = %s\n", ezxml_child(e, "type")->name, ezxml_child(e, "type")->txt);

		p = ezxml_attr(ezxml_child(e, "addr"), "count");
		printf("%s = %s\n", ezxml_child(e, "addr")->name, p);
		printf("%s\n", e->name);
	}

	return 0;
}

