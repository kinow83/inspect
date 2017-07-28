

static void config_free(struct config_t* config) {
	struct action_t *tmpa;

	if (!config) return;

	a = config->action;
	while (a) {
		tmpa = a->next;
		free(a);
		a = tmpa;
	}

	free(config);
}

static struct config_t* xml_action_parser(ezxml_t xml) {
	struct action_t *a;

	a = (struct action_t*)calloc(1, sizeof(*a));

	// enable
	a->enable = 1;
	ezxml2int(xml, "enable", a->enable);

	// desc
	ezxml2chararr(xml, "name", a->name);

	// dwell
	ezxml2int(xml, "dwell", a->dwell);

	// channel
	ezxml2int(xml, "channle", a->channle);

	// check mandatory config
	if (a->enable == 0 || a->name[0] == 0 
		|| a->channel == 0 || a->dwell == 0) {
		free(a);
		return NULL;
	}

	// IEEE 802.11 mac frame type
	ezxml2cv(xml, "type", a->type);

	// IEEE 802.11 mac frame subtype
	ezxml2cv(xml, "subtype", a->subtype);

	// from ds / to ds
	ezxml2cv(xml, "fromds", a->fromds);
	ezxml2cv(xml, "tods", a->tods);



	return a;
}

static struct config_t* xml_config_parser(const char* filename, ezxml_t xml) {
	ezxml_t e;
	uint32_t version = 0;
	config_t *config;
	struct action_t *action;

	config = (struct config_t*)calloc(1, sizeof(*config));

	ezxml2int(xml, "version", version);

	if (version > CUR_VERSION) {
		config_free(config);
		echo.f("version mismatch current %d in %s", 
		CUR_VERSION, filename);
	}

	// multi action in config.
	for (e = ezxml_child(xml, "action"); e; e = e->next) {
		action = xml_action_parser(xml);
		if (a) {
			if (config->action) {
				action->next = config->action->next;
				config->action = action;
			} 
			else {
				config->action = action;
			}
		}
	}

	if (!config->action) {
		config_free(config);
		echo.f("empty config file %s", filename);
	}
	return config;
}

/*
 * name: xml_config_load
 * desc: XML 설정 파일을 불러온다.
 */
static struct config_t* xml_config_load(const char* filename) {
	struct config_t *config;
	ezxml_t xml;
	
	xml  = ezxml_parse_file(filename);
	if (!xml) {
		echo.f("%s is not config file", filename);
	}

	config = xml_config_parser(filename, xml);

	ezxml_free(xml);
	return config;
}

static void xml_action_debug(struct action_t *action) {
	echo.i("no = %d", action->no);
	echo.i("name = %s", action->name);
	echo.i("channel = %d", action->channel);
	echo.i("dwell = %d", action->dwell);

	if (cv_enabled(action->type)) {
		echo.i("type = %d", action->type);
	}
	if (cv_enabled(action->subtype)) {
		echo.i("subtype = %d", action->subtype);
	}
	if (cv_enabled(action->tods)) {
		echo.i("tods = %d", action->tods);
	}
	if (cv_enabled(action->fromds)) {
		echo.i("fromds = %d", action->fromds);
	}
	if (cv_enabled(action->addr_count)) {
		echo.i("addr_count = %d", action->addr_count);
		switch (action->addr_count) {
		case 1:
			echo.i("addr1 = "_MAC_FMT_, _MAC_FMT_FILL_(action->addr1));
		case 2:
			echo.i("addr2 = "_MAC_FMT_, _MAC_FMT_FILL_(action->addr2));
		case 3:
			echo.i("addr3 = "_MAC_FMT_, _MAC_FMT_FILL_(action->addr3));
		case 4:
			echo.i("addr4 = "_MAC_FMT_, _MAC_FMT_FILL_(action->addr4));
			break;
		}
	}
	else {
		if (cv_enabled(action->any_addr)) {
			echo.i("any_addr = "_MAC_FMT_, _MAC_FMT_FILL_(action->any_addr));
		}
		else {
			if (cv_enabled(action->ap_addr)) {
				echo.i("ap_addr = "_MAC_FMT_, _MAC_FMT_FILL_(action->ap_addr));
			}
			if (cv_enabled(action->st_addr)) {
				echo.i("st_addr = "_MAC_FMT_, _MAC_FMT_FILL_(action->st_addr));
			}
		}
	}
	if (cv_enabled()) {
	}
	if (cv_enabled()) {
	}
	if (cv_enabled()) {
	}
	if (cv_enabled()) {
	}
	if (cv_enabled()) {
	}
	if (cv_enabled()) {
	}
}

static void xml_config_debug(struct config_t* config) {
}

struct config_operation xml_config = {
	.config_load = xml_config_load,
	.config_free = config_free,
	.config_debug = xml_config_debug,
};
