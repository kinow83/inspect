
typedef struct Xml_output_t {
	char *config_filename;
	

};

static void xml_output(Action_t *action, u8 *h80211)
{
	ezxml_t xml = ezxml_new("result");
	
}

void init_xml_output_module()
{
	register_output_module("xml", xml_output);
}
