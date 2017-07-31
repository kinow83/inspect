# Makefile
#
# Copyright 2017 Kang seok ju <kinow83@gmail.com>
#

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I ../ -I ../../lib/

lib_LTLIBRARIES = libxmloutput.la

libxmloutput_la_SOURCES = xml_output.c

libxmloutput_la_CFLAGS = -D_REENTRANT
libxmloutput_la_LDFLAGS = -version-info 1:0:0

LDADD = 
