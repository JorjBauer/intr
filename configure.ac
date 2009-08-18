# Process this file with autoconf to produce a configure script.
AC_INIT(intr, 0.1-090817, jorj@jorj.org)
AC_CONFIG_HEADER([config.h])

# Check for programs we'll need in order to build and install...
AC_PROG_CC

AC_HEADER_STDC
AC_CHECK_HEADER(unistd.h)
AC_CHECK_HEADER(signal.h)
AC_HEADER_SYS_WAIT
AC_CHECK_DECLS(sys_signame,,, [#include <signal.h>])


AC_CONFIG_FILES([Makefile])

AC_OUTPUT
