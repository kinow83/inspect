# Makefile
#
# Copyright 2017 Kang seok ju <kinow83@gmail.com>
#
AUTOMAKE_OPTIONS = foreign no-dependencies

SUBDIRS = osdep ezxml match_modules output_modules parser_modules rtx_modules

bin_PROGRAMS = inspect

inspect_SOURCES = h80211_struct.h version.h types.h format.h \
alloc.c alloc.h \
convert.c convert.h \
inspect.c \
log.c log.h \
match.c match.h \
output.c output.h \
parser.c parser.h \
resource.c resource.h \
rtx.c rtx.h \
strings.c strings.h \
module.c module.h 


inspect_LDADD = match_modules/libmatch.a \
output_modules/liboutput.a \
parser_modules/libparser.a \
rtx_modules/librtx.a \
ezxml/libezxml.a \
osdep/libosdep.a

inspect_CFLAGS = -g

