# Makefile
#
# Copyright 2017 Kang seok ju <kinow83@gmail.com>
#
AUTOMAKE_OPTIONS = foreign no-dependencies

SUBDIRS = osdep ezxml match_modules output_modules parser_modules rtx_modules cmdutil

sbin_PROGRAMS = inspect

inspect_SOURCES = h80211_struct.h version.h typedef.h format.h \
alloc.c alloc.h \
convert.c convert.h \
inspect.c \
log.c log.h \
match.c match.h \
output.c output.h \
parser.c parser.h \
resource.c resource.h \
rtx.c rtx.h \
string_util.c string_util.h \
module.c module.h \
perform.h


inspect_LDADD = match_modules/libmatch.a \
output_modules/liboutput.a \
parser_modules/libparser.a \
rtx_modules/librtx.a \
ezxml/libezxml.a \
osdep/libosdep.a

inspect_CFLAGS = -g -DDEBUG

bashcompletiondir = /etc/bash_completion.d/
bashcompletion_DATA = bash_completion/inspect
