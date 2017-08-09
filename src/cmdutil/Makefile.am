# Makefile
#
# Copyright 2017 Kang seok ju <kinow83@gmail.com>
#
AUTOMAKE_OPTIONS = foreign no-dependencies

sbin_PROGRAMS = cmdutil

cmdutil_SOURCES = cmdutil.c cmdutil.h \
iw.c iw.h \
../string_util.c ../string_util.h

cmdutil_CFLAGS = -g -DDEBUG

AM_CPPFLAGS = -I../

bashcompletiondir = /etc/bash_completion.d/
bashcompletion_DATA = bash_completion/cmdutil

