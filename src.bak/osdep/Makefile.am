# Makefile
#
# Copyright 2017 Kang seok ju <kinow83@gmail.com>
#
AUTOMAKE_OPTIONS = foreign no-dependencies subdir-objects

noinst_LIBRARIES = libosdep.a

libosdep_a_SOURCES = linux.c linux_tap.c \
osdep.h osdep.c crctable_osdep.h \
byteorder.h \
radiotap/radiotap.h radiotap/radiotap.c radiotap/radiotap_iter.h radiotap/platform.h \
common.c common.h \
file.c \
network.c network.h

#libosdep_a_LIBADD = -lpcap

AM_CPPFLAGS = -I../
