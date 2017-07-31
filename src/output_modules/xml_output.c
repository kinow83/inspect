#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "alloc.h"
#include "output.h"
#include "ezxml/ezxml.h"


static void do_xml_output(Action_t *action, u8 *h80211)
{
	ezxml_t xml = ezxml_new("result");

}

static void init_xml_output(void)
{
	echo.d("init_xml_output");
}

static void done_xml_output(void)
{
	echo.d("done_xml_output");
}

void init_xml_output_module()
{
	Output_operations_t op = {
			.init_output = init_xml_output,
			.do_output = do_xml_output,
			.done_output = done_xml_output,
	};

	register_output_module("xml", &op);
}
