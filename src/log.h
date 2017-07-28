
#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>
#include <stdarg.h>

#define L_FATAL 0
#define L_ERROR 1
#define L_INFO  2
#define L_DEBUG 3
#define L_ALL   (L_DEBUG+1)

struct log_ctx {
	void (*f)(const char* fmt, ...);
	void (*e)(const char* fmt, ...);
	void (*i)(const char* fmt, ...);
	void (*d)(const char* fmt, ...);

	void (*hf)(uint8_t* hex, size_t len, const char* fmt, ...);
	void (*he)(uint8_t* hex, size_t len, const char* fmt, ...);
	void (*hi)(uint8_t* hex, size_t len, const char* fmt, ...);
	void (*hd)(uint8_t* hex, size_t len, const char* fmt, ...);
};

extern struct log_ctx L;
extern int log_level;
extern struct log_ctx echo;

#endif
