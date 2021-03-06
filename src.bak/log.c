#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "log.h"

/*
<List of colors for prompt and Bash>

txtblk='\e[0;30m' # Black - Regular
txtred='\e[0;31m' # Red
txtgrn='\e[0;32m' # Green13
txtylw='\e[0;33m' # Yellow
txtblu='\e[0;34m' # Blue
txtpur='\e[0;35m' # Purple
txtcyn='\e[0;36m' # Cyan
txtwht='\e[0;37m' # White
bldblk='\e[1;30m' # Black - Bold
bldred='\e[1;31m' # Red
bldgrn='\e[1;32m' # Green
bldylw='\e[1;33m' # Yellow
bldblu='\e[1;34m' # Blue
bldpur='\e[1;35m' # Purple
bldcyn='\e[1;36m' # Cyan
bldwht='\e[1;37m' # White
unkblk='\e[4;30m' # Black - Underline
undred='\e[4;31m' # Red
undgrn='\e[4;32m' # Green
undylw='\e[4;33m' # Yellow
undblu='\e[4;34m' # Blue
undpur='\e[4;35m' # Purple
undcyn='\e[4;36m' # Cyan
undwht='\e[4;37m' # White
bakblk='\e[40m'   # Black - Background
bakred='\e[41m'   # Red
bakgrn='\e[42m'   # Green
bakylw='\e[43m'   # Yellow
bakblu='\e[44m'   # Blue
bakpur='\e[45m'   # Purple
bakcyn='\e[46m'   # Cyan
bakwht='\e[47m'   # White
txtrst='\e[0m'    # Text Reset
 */

#ifndef LOG_NO_COLOR
#define VTC_BLUE "\x1b[34m"	  //!< Colour following text blue.
#define VTC_RED "\x1b[31m"	  //!< Colour following text red.
#define VTC_GREEN "\x1b[32m"  //!< Colour following text creen.
#define VTC_YELLOW "\x1b[33m" //!< Colour following text yellow.
#define VTC_BOLD "\x1b[1m"	  //!< Embolden following text.
#define VTC_WHITE "\x1b[37m"  //!< Colour following text white.

#define VTC_RESET "\x1b[0m" //!< Reset terminal text to default style/colour.
#endif

static const char *LOG_STR[] = {
	"FATAL",
	"ERROR",
	"INFO ",
	"DEBUG",
};

#define LOG_BUFSIZE 10240

static void console_log(int lv, const char *fmt, va_list ap,
						u8 *hex, size_t len, const char *color, bool bold)
{
	struct tm t_now;
	time_t now;
	char strtime[256];
	char buf[LOG_BUFSIZE];
	FILE *fp;
	const char *lv_str;
	int n, i;
	static char init_env = 0;

	fp = stdout;
	if (!fp)
	{
		return;
	}

	if (init_env == 0)
	{
		// os default file buffering.
		setvbuf(fp, (char *)NULL, _IOLBF, (size_t)0);
		init_env = 1;
	}

	lv_str = LOG_STR[lv];
	now = time(NULL);
	localtime_r(&now, &t_now);

	if (log_level >= lv)
	{
		strftime(strtime, sizeof(strtime), "%H:%M:%S", &t_now);
		n = vsnprintf(buf, sizeof(buf), fmt, ap);

		if (hex)
		{
			for (i = 0; i < (int)len; i++)
			{
				n += snprintf(buf + n, sizeof(buf) - n, " %02x", hex[i]);
			}
		}

		if (bold == true)
		{
			fprintf(fp, "%s", VTC_BOLD);
		}
		if (hex)
		{
			fprintf(fp, "%s%s: %s: (len:%ld) %s\n",
					color, strtime, lv_str, len, buf);
		}
		else
		{
			fprintf(fp, "%s%s: %s: %s\n", color, strtime, lv_str, buf);
		}
		fprintf(fp, "%s", VTC_RESET);

		if (lv == L_FATAL)
		{
			exit(1);
		}
	}
}

static void console_out(const char *fmt, ...)
{
	char buf[LOG_BUFSIZE];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	fprintf(stderr, "%s\n", buf);
	va_end(ap);
}

static void CONSOLE_OUT(const char *fmt, ...)
{
	char buf[LOG_BUFSIZE];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	fprintf(stderr, "%s%s\n%s", VTC_BOLD, buf, VTC_WHITE);
	va_end(ap);
}

/*
 * normal
 */
static void console_fatal(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	console_log(L_FATAL, fmt, ap, NULL, 0, VTC_YELLOW, false);
	va_end(ap);
}

static void console_error(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	console_log(L_ERROR, fmt, ap, NULL, 0, VTC_RED, false);
	va_end(ap);
}

static void console_info(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	console_log(L_INFO, fmt, ap, NULL, 0, VTC_GREEN, false);
	va_end(ap);
}

static void console_debug(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	console_log(L_DEBUG, fmt, ap, NULL, 0, VTC_WHITE, false);
	va_end(ap);
}

static void console_hex_fatal(u8 *hex, size_t len, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	console_log(L_FATAL, fmt, ap, hex, len, VTC_YELLOW, false);
	va_end(ap);
}

/*
 * bold
 */
static void CONSOLE_FATAL(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	console_log(L_FATAL, fmt, ap, NULL, 0, VTC_YELLOW, true);
	va_end(ap);
}

static void CONSOLE_ERROR(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	console_log(L_ERROR, fmt, ap, NULL, 0, VTC_RED, true);
	va_end(ap);
}

static void CONSOLE_INFO(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	console_log(L_INFO, fmt, ap, NULL, 0, VTC_GREEN, true);
	va_end(ap);
}

static void CONSOLE_DEBUG(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	va_end(ap);
	console_log(L_DEBUG, fmt, ap, NULL, 0, VTC_WHITE, true);
}

/*
 * hex
 */
static void console_hex_error(u8 *hex, size_t len, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	console_log(L_ERROR, fmt, ap, hex, len, VTC_RED, false);
	va_end(ap);
}

static void console_hex_info(u8 *hex, size_t len, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	console_log(L_INFO, fmt, ap, hex, len, VTC_GREEN, false);
	va_end(ap);
}

static void console_hex_debug(u8 *hex, size_t len, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	console_log(L_DEBUG, fmt, ap, hex, len, VTC_WHITE, false);
	va_end(ap);
}

int log_level = L_ALL;

struct log_ctx echo = {
	.out = console_out,
	.OUT = CONSOLE_OUT,

	.F = CONSOLE_FATAL,
	.E = CONSOLE_ERROR,
	.I = CONSOLE_INFO,
	.D = CONSOLE_DEBUG,

	.f = console_fatal,
	.e = console_error,
	.i = console_info,
	.d = console_debug,

	.fx = console_hex_fatal,
	.ex = console_hex_error,
	.ix = console_hex_info,
	.dx = console_hex_debug,
};
