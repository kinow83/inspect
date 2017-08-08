/*
 * aircrack_utils.c
 *
 *  Created on: 2017. 8. 8.
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "strings.h"

/* Macros for min/max.  */
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MAX_CMD_LINE 1024

static char *linux_search_inside(const char *dir, const char *filename)
{
	char *ret;
	DIR *dp;
	char *curfile;
	size_t len;
	int totlen;
	struct dirent *ep;
	struct stat sb;

	dp = opendir(dir);
	if (!dp) return NULL;

	len = strlen(filename);
	totlen = strlen(dir) + 256 + 2;
	curfile = (char*) calloc(1, totlen);

	while ((ep = readdir(dp)) != NULL) {
		memset(curfile, 0, totlen);
		snprintf(curfile, totlen, "%s/%s", dir, ep->d_name);
		if ((strlen(ep->d_name) == len) && (strcmp(ep->d_name, filename) == 0)) {
			(void) closedir(dp);
			return curfile;
		}
		if ((lstat(curfile, &sb) == 0)
				&& S_ISDIR(sb.st_mode)
				&& !S_ISLNK(sb.st_mode)) {

			if (strcmp(".", ep->d_name) && strcmp("..", ep->d_name)) {
				// recursive call
				ret = linux_search_inside(curfile, filename);
				if (ret) {
					(void) closedir(dp);
					free(curfile);
					return ret;
				}
			}
		}
	}
	(void) closedir(dp);
	free(curfile);
	return NULL;
}

char *linux_path(const char *cmd)
{
	char *path;
	int i, nbelems;
	static const char *paths[] = {
			".",
			"/sbin",
			"/usr/sbin",
			"/usr/local/sbin",
			"/bin",
			"/usr/bin",
			"/usr/local/bin",
	};

	nbelems = sizeof(paths) / sizeof(char*);
	for (i = 0; i < nbelems; i++) {
		path = linux_search_inside(paths[i], cmd);
		if (path) {
			return path;
		}
	}
	return NULL;
}

#define MISS_STR_STR (((void *)0)-1)
static char *str_str(const char *str, const char *needle)
{
	int i, cnt;
	char *pos;
	char *min = MISS_STR_STR;

	cnt = strlen(needle);

	for (i=0; i<cnt; i++) {
		pos = strchr(str, needle[i]);
		if (pos) {
			if (pos < min) {
				min = pos;
			}
		}
	}
	return min;
}



struct iw_info_t {
	char dev[32];
	struct iw_info_t *next;
};

void free_iw_info(struct iw_info_t *list) {
	struct iw_info_t *tmp, *cur;

	cur = list;
	while (cur) {
		tmp = cur->next;
		free(cur);
		cur = tmp;
	}
}

size_t num_iw_info(struct iw_info_t *list) {
	struct iw_info_t *cur;
	size_t cnt = 0;

	cur = list;
	while (cur) {
		cnt++;
		cur = cur->next;
	}
	return cnt;
}

struct iw_info_t *get_wifi_devs()
{
	struct iw_info_t *info, *first, *last;
	FILE *out;
	char *path;
	char buf[MAX_CMD_LINE];
	char cmd[MAX_CMD_LINE];
	char *pos;

	path = linux_path("iwconfig");
	if (!path) {
		return NULL;
	}

	snprintf(cmd, sizeof(cmd), "%s 2>/dev/null | grep 'IEEE 802.11'", path);
	free(path);
//	printf("cmd: %s\n", cmd);

	out = popen(cmd, "r");
	if (!out) return NULL;

	first = NULL;
	while (fgets(buf, sizeof(buf), out)) {

		pos = str_str(buf, " \r\n");
		if (pos != MISS_STR_STR) {
			info = (struct iw_info_t *)malloc(sizeof(struct iw_info_t));
			memset(info, 0, sizeof(struct iw_info_t));

			snprintf(info->dev, MIN(sizeof(info->dev), pos-buf+1), "%s", buf);

			if (first == NULL) {
				first = info;
			} else {
				last = first;
				while (last->next) {
					last = last->next;
				}
				last->next = info;
			}
		}
	}
	pclose(out);
	return first;
}

struct iw_info_t *get_wifi_monitor_devs(const char *iw_prefix)
{
	struct iw_info_t *info, *first, *last;
	FILE *out;
	char *path;
	char buf[MAX_CMD_LINE];
	char cmd[MAX_CMD_LINE];
	char *pos;

	path = linux_path("iwconfig");
	if (!path) {
		return NULL;
	}

	snprintf(cmd, sizeof(cmd), "%s 2>/dev/null | grep ^%s | awk '{print $1}'", path, iw_prefix);
	free(path);
//	printf("cmd: %s\n", cmd);

	out = popen(cmd, "r");
	if (!out) return NULL;

	first = NULL;
	while (fgets(buf, sizeof(buf), out)) {

		pos = str_str(buf, " \r\n");
		if (pos != MISS_STR_STR) {

			info = (struct iw_info_t *)malloc(sizeof(struct iw_info_t));
			memset(info, 0, sizeof(struct iw_info_t));

			snprintf(info->dev, MIN(sizeof(info->dev), pos-buf+1), "%s", buf);

			if (first == NULL) {
				first = info;
			} else {
				last = first;
				while (last->next) {
					last = last->next;
				}
				last->next = info;
			}
		}
	}
	pclose(out);
	return first;
}

bool airmon_ng_start(const char *dev)
{
	char *path;
	char cmd[MAX_CMD_LINE];

	path = linux_path("airmon-ng");
	if (!path) {
		return false;
	}
	snprintf(cmd, sizeof(cmd), "%s start %s 1>&2 > /dev/null", path, dev);
//	printf("%s\n", cmd);

	return system(cmd) ? false : true;
}

bool airmon_ng_stop(const char *dev)
{
	char *path;
	char cmd[MAX_CMD_LINE];

	path = linux_path("airmon-ng");
	if (!path) {
		return false;
	}
	snprintf(cmd, sizeof(cmd), "%s stop %s 1>&2 > /dev/null", path, dev);
//	printf("%s\n", cmd);

	return system(cmd) ? false : true;
}

bool airmon_ng_multi_stop(int count, ...)
{
	int i;
	va_list ap;
	char *dev;
	bool ok = true;

	va_start(ap, count);
	for (i=0; i<count; i++) {
		dev = va_arg(ap, char*);
		if (airmon_ng_stop(dev) == false) {
			ok = false;
		}
	}
	va_end(ap);
	return ok;
}

bool airmon_ng_wi_stop(struct iw_info_t *list)
{
	struct iw_info_t *cur;
	bool ok = true;

	cur = list;
	while (cur) {
		if (airmon_ng_stop(cur->dev) == false) {
			ok = false;
		}
		cur = cur->next;
	}
	return ok;
}

bool airmon_ng_multi_start(int count, ...)
{
	int i;
	va_list ap;
	char *dev;
	bool ok = true;

	va_start(ap, count);
	for (i=0; i<count; i++) {
		dev = va_arg(ap, char*);
		if (airmon_ng_start(dev) == false) {
			ok = false;
		}
	}
	va_end(ap);
	return ok;
}

bool airmon_ng_wi_start(struct iw_info_t *list)
{
	struct iw_info_t *cur;
	bool ok = true;

	cur = list;
	while (cur) {
		if (airmon_ng_start(cur->dev) == false) {
			ok = false;
		}
		cur = cur->next;
	}
	return ok;
}

int main()
{
	struct iw_info_t *list, *cur;

	list = get_wifi_monitor_devs("mon");
	airmon_ng_wi_stop(list);
	free(list);

	list = get_wifi_devs();

	if (num_iw_info(list) < 2) {
		fprintf(stderr, "wifi dev count than 2\n");
		free_iw_info(list);
		return -1;
	}
	airmon_ng_wi_start(list);
	free_iw_info(list);

	list = get_wifi_monitor_devs("mon");
	cur = list;
	while (cur) {
		printf("monitor dev = %s\n", cur->dev);
		cur = cur->next;
	}
	free_iw_info(list);

	return 0;
}
