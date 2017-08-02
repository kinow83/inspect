# Makefile
#
# Copyright 2017 Kang seok ju <kinow83@gmail.com>
#
AUTOMAKE_OPTIONS = foreign no-dependencies

SUBDIRS = osdep ezxml match_modules output_modules parser_modules

bin_PROGRAMS = inspect

inspect_SOURCES = alloc.h alloc.c \
format.h \
inspect.c \
match.h match.c \
output.h output.c \
parser.h parser.c \
log.h log.c \
strings.c string.h \
convert.c convert.h \
resource.c resource.h \
h80211_struct.h version.h types.h


inspect_LDADD = match_modules/libmatch.a \
output_modules/liboutput.a \
parser_modules/libparser.a \
ezxml/libezxml.a 

