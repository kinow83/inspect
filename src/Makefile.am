# Makefile
#
# Copyright 2017 Kang seok ju <kinow83@gmail.com>
#

AUTOMAKE_OPTIONS = foreign

#SUBDIRS = match_modules output_modules parser_modules

bindir = /usr/bin
bin_PROGRAMS = inspect

#confdir = conf
#conf_DATA = test.xml

LDADD = -lezxml

inspect_SOURCES = inspect.c

inspect_CFLAGS = -D_REENTRANT -Lezxml/.libs/

