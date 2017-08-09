#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([inspect], [1.0.0], [kinow83@gmail.com])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/inspect.c])
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC_STDC
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_RANLIB
AC_C_BIGENDIAN

# Checks for libraries.
AC_CHECK_LIB([pthread], [main])
AC_CHECK_HEADERS([pcap.h])
AC_CHECK_LIB([pcap], [main])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC

#AC_DEFINE([HAVE_DEBUG], [1], [debug mode])

AC_CONFIG_FILES([Makefile
				src/Makefile
				src/ezxml/Makefile
				src/output_modules/Makefile
				src/parser_modules/Makefile
				src/match_modules/Makefile
				src/rtx_modules/Makefile
				src/osdep/Makefile
				src/cmdutil/Makefile])

AC_OUTPUT

