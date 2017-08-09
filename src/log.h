
#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>
#include <stdlib.h>

#include "typedef.h"

#define L_FATAL 0
#define L_ERROR 1
#define L_INFO  2
#define L_DEBUG 3
#define L_ALL   (L_DEBUG+1)

struct log_ctx {
	void (*out)(const char *fmt, ...);
	void (*OUT)(const char *fmt, ...);

	void (*f)(const char *fmt, ...);
	void (*e)(const char *fmt, ...);
	void (*i)(const char *fmt, ...);
	void (*d)(const char *fmt, ...);

	void (*F)(const char *fmt, ...);
	void (*E)(const char *fmt, ...);
	void (*I)(const char *fmt, ...);
	void (*D)(const char *fmt, ...);

	void (*hf)(u8 *hex, size_t len, const char *fmt, ...);
	void (*he)(u8 *hex, size_t len, const char *fmt, ...);
	void (*hi)(u8 *hex, size_t len, const char *fmt, ...);
	void (*hd)(u8 *hex, size_t len, const char *fmt, ...);
};

extern int log_level;
extern struct log_ctx echo;

#endif
