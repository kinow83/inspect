#ifndef _PARSER_
#define _PARSER_

#include <stdint.h>
#include "types.h"
#include "format.h"
#include "version.h"
#include "osdep/osdep.h"

/*
 * cv_def: Config Value Define.
 */
#define cv_def(name, type) \
    type name; \
	u8 name##_en;
/*
 * cv_arr_def: Config Value Array Define.
 */
#define cv_arr_def(name, type, size) \
    type name[size]; \
	u8 name##_en;

#define cv_enabled(name) \
	({ name##_en == 1; })

#define cv_enable(name) \
	do { name##_en = 1; } while (0)

#define cv_disable(name) \
	do { name##_en = 0; } while (0)

#define cv_disable(name) \
	do { name##_en = 0; } while (0)

/*
 * 802.11 Information Element.
 */
typedef struct Tag_t {
	u8 tag;
	u8 len;
	u8 *data;
	struct Tag_t *next;
} Tag_t;

// predefine.
typedef struct Config_t Config_t;

/*
 * shooter or capture config.
 * support multi action.
 */
typedef struct Action_t {
	Config_t *config;
	u32 no;				// sequence number
	u8 enable;
	char *name;
	u8 channel;
	u32 dwell;
	cv_def(version, u8);   // 802.11 version
	cv_def(type, u8);      // frame type
	cv_def(subtype, u8);   // frame subtype
	cv_def(tods, u8);      // tods
	cv_def(fromds, u8);    // fromds
	cv_def(addr_count, u8);// mac address count
	cv_def(addr1, mac_t);       // mac address1
	cv_def(addr2, mac_t);       // mac address2
	cv_def(addr3, mac_t);       // mac address3
	cv_def(addr4, mac_t);       // mac address4
	cv_def(ap_addr, mac_t);     // AP mac address (aka. BSSID)
	cv_def(st_addr, mac_t);		// ST mac address (station)
	cv_def(any_addr, mac_t);	// ANY mac address (any address capture)
	struct Tag_t *tags;
	struct Action_t *next;
} Action_t;

typedef struct Config_t  {
	u32 version;  // config version
	char *name;
	u32 extra_dwell;
	struct wif *capture_wif;
	struct wif *shooter_wif;
	struct Action_t *action;
} Config_t;

typedef struct Config_operations_t {
	void (*init_parser)(void);
	Config_t* (*do_parser)(char *);
	void (*done_parser)(void);
} Config_operations_t;

typedef struct Config_module_t {
	char *config_name;
	Config_operations_t op;
	struct Config_module_t *next;
} Config_module_t;


void init_parser(void);
Config_t *do_parser(const char *config_name, char *args);
void done_parser(void);
void register_parser_module(const char *, Config_operations_t *);
void setup_parser_modules(void);

void free_tags(Tag_t *tag);
void free_actions(Action_t *action);
void free_config(Config_t *config);
void free_config_modules(Config_module_t *mod);
Action_t *max_dwell_action(Config_t *config);
void debug_action(Action_t *action);
void debug_config(Config_t *config);

#endif
