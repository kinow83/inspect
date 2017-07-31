# Makefile
#
# Copyright 2017 Kang seok ju <kinow83@gmail.com>
#
AUTOMAKE_OPTIONS = foreign no-dependencies

bin_PROGRAMS = inspect

inspect_SOURCES = inspect.c match.c output.c parser.c log.c alloc.c 

inspect_LDADD = ezxml/libezxml.a \
				match_modules/libmatch.a \
				output_modules/liboutput.a \
				parser_modules/libparser.a

SUBDIRS = ezxml match_modules output_modules parser_modules

LDADD = -lezxml

inspect_CFLAGS = -D_REENTRANT
inspect_LDFLAGS = @INCLUDES@
