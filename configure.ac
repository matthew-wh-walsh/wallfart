dnl
dnl configure.ac for WallFArt
dnl
m4_define([wallfart_major_version], [0])
m4_define([wallfart_minor_version], [3])
m4_define([wallfart_micro_version], [7])
m4_define([wallfart_version],
          [wallfart_major_version.wallfart_minor_version.wallfart_micro_version])

AC_PREREQ(2.67)
AC_INIT([wallfart], [wallfart_version], [matthew.sonofloreATgmail.com])
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([src/wallfart.c])
AC_CONFIG_HEADERS([config.h])

dnl
dnl the standard C stuff
dnl
AM_PROG_CC_C_O
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_CC_C99
AC_CHECK_HEADERS([string.h stdlib.h stdio.h math.h time.h errno.h]) 
CFLAGS="$CFLAGS"
LIBS="$LIBS"

dnl
dnl gtk/glib/pkg-config etc...  
dnl
AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal, glib-genmarshal)
PKG_PROG_PKG_CONFIG
GTK_CFLAGS=""
GTK_LIBS=""

if test -n "$PKG_CONFIG" ; then
dnl	PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.0.0]) 
	PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.0.0], [have_ac_gtk=yes], [have_ac_gtk=no])
else
	echo ""
	echo "     I need pkg-config (a gtk+-2.0 development environment) to build"
	echo ""
	echo "     ABORT! ABORT... Danger, Will Robinson, danger... abooorrrrrt..."
	echo ""
	exit
fi

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
