/*
 * resource.h
 *
 *  Created on: 2017. 8. 2.
 *      Author: root
 */

#ifndef SRC_RESOURCE_H_
#define SRC_RESOURCE_H_

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include "format.h"
#include "osdep/osdep.h"
#include "typedef.h"


typedef struct Module_option_t {
	char *name;
	char *options;
	struct Module_option_t *next;
} Module_option_t;

typedef struct Module_option_list_t {
	Module_option_t *output;
	Module_option_t *parser;
	Module_option_t *match;
	Module_option_t *rtx;
} Module_option_list_t;

void free_module_option(Module_option_t *);
Module_option_t* new_module_option(const char *args);
size_t get_module_option_count(Module_option_t *mopt);

const char *get_80211_type_name(u8 type);
const char *get_80211_subtype_name(u8 type, u8 subtype);
bool get_80211_type_code(const char *typename, u8 *type);
bool get_80211_subtype_code(
		const char *typename, const char *subtypename, u8 *type, u8 *subtype);
void debug_h80211_type_names(void);

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
enum {
	TAG_TYPE_STR = 0,
	TAG_TYPE_HEX = 1,
};
typedef struct Tag_t {
	u8 id;
	u8 len;
	u8 type;
	u8 *data;
	struct Tag_t *next;
} Tag_t;

typedef struct Action_t Action_t;
/************************************
 * struct Action_details_t
 ************************************/
typedef struct Action_details_t {
	Action_t *action;
	char *name;
	u32 id;
	u8 malformable; // malformable

	cv_def(version, u8);     // 802.11 version
	cv_def(ibss, u8);
	cv_def(protect, u8);
	cv_def(duration, u16);   // duration
	cv_def(type, u8);        // frame type
	cv_def(subtype, u8);     // frame subtype
	cv_def(fromds, u8);      // fromds
	cv_def(tods, u8);        // tods
	cv_def(addr1, mac_t);    // mac address1
	cv_def(addr2, mac_t);    // mac address2
	cv_def(addr3, mac_t);    // mac address3
	cv_def(addr4, mac_t);    // mac address4
	cv_def(da, mac_t);       // dst address
	cv_def(sa, mac_t);       // src address
	cv_def(any_addr, mac_t); // ANY mac address (any address capture)
	cv_def(deauth_reason, u16); // deauth reason code
	struct Tag_t *tags;
	struct Action_details_t *next;
} Action_details_t;

/*
 * shooter or capture config.
 * support multi action.
 */
typedef struct Config_t Config_t;
/************************************
 * struct Action_t
 ************************************/
typedef struct Action_t {
	Config_t *config;
	u32 id; // sequence number
	u8 enable;
	char *name;
	u8 channel;
	u32 interval;
	u32 interval_count;
	struct Action_t *next;

	Action_details_t *shooter;
	Action_details_t *capture;
	void *opaque;
} Action_t;

/************************************
 * struct Config_t
 ************************************/
typedef struct Config_t  {
	char *filename;
	u32 version;  // config version
	char *name;
	u32 ext_interval;
	struct wif *capture_wif;
	struct wif *shooter_wif;
	struct Action_t *action;
} Config_t;

void free_tags(Tag_t *tag);
void free_action_details(Action_details_t *detail);
void free_actions(Action_t *action);
void free_config(Config_t *config);

void debug_tags(Tag_t *tag);
void debug_action_details(Action_details_t *detail);
void debug_actions(Action_t *action);
void debug_config(Config_t *config);
Action_t *get_max_dwell(Config_t *config);
void sort_actions(Action_t **action);

Action_t *get_max_interval(Config_t *config);
int get_action_count(Config_t *config);
int get_capture_count(Action_t *action);
int get_shooter_count(Action_t *action);

bool verify_action(Action_t *action);
bool verify_config(Config_t *config);

Tag_t *find_tag(Tag_t *first, u8 id);
Tag_t *find_tag_vendor(Tag_t *first, u8 *oui);
Tag_t *last_tag(Tag_t *first);
Tag_t *new_tag(Tag_t *prev, u8 id, u8 len, u8 type, u8 *data);
Tag_t *new_sort_tags(Tag_t *first);

pthread_t run_or_thread(Config_t *, bool, void *(*fp)(void *));

#endif /* SRC_RESOURCE_H_ */
