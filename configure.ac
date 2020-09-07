#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([vmtop], [1.0], [virt@openeuler.org])
AC_CONFIG_SRCDIR([src/vmtop.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AM_INIT_AUTOMAKE([subdir-objects])
CC="$CC -std=gnu11"

# Checks for libraries.
AC_CHECK_LIB(ncurses, initscr, [], [echo "missing ncurses support!";exit -1])
NCURSES_LIBS="-lncurses"
AC_SUBST([NCURSES_LIBS])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset realpath strstr strtoul])

# AC_ARGS_ENABLES
AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [debug program(default is disable)])],
  [CFLAGS="-g -O0"],
  [CFLAGS="-g -O2"]
)

AC_ARG_ENABLE([secure-build],
  [AS_HELP_STRING([--enable-secure-build], [build project with secure flags])],
  [CFLAGS="${CFLAGS} -Wl,-z,relro,-z,now -fPIE -fPIC -pie -Werror -Wall -ftrapv -fstack-protector-all -D_FORTIFY_SOURCE=2 -O2"],[]
)

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
