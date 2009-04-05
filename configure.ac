dnl vim: set sw=4 sts=4 ts=4 noet ft=config foldmethod=marker foldmarker={{{,}}} :

dnl {{{ program, version
AC_PREREQ(2.59)
AC_INIT([src/main.cpp])

VERSION_MAJOR=0
VERSION_MINOR=2
VERSION_FULL="$VERSION_MAJOR.$VERSION_MINOR"
VERSION="$VERSION_FULL"

AC_SUBST([VERSION_MAJOR])
AC_SUBST([VERSION_MINOR])
AC_SUBST([VERSION_FULL])

AM_INIT_AUTOMAKE(envtag, [$VERSION_FULL])

dnl {{{ git revision
AC_MSG_CHECKING([for git head])
if test -d "${GIT_DIR:-${ac_top_srcdir:-./}/.git}" ; then
	GITHEAD=`git describe 2>/dev/null`
	if test -z ${GITHEAD} ; then
		GITHEAD=`git rev-parse --short HEAD`
	fi
	if test -n "`git diff-index -m --name-only HEAD`" ; then
		GITHEAD=${GITHEAD}-dirty
	fi
fi
AC_MSG_RESULT([$GITHEAD])
AC_SUBST([GITHEAD])
dnl }}}
dnl }}}

dnl {{{ toolchain checks
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
dnl }}}

dnl {{{ check for taglib
PKG_CHECK_MODULES([taglib], [taglib >= 1.5],,
				  [AC_MSG_ERROR([envtag requires taglib-1.5 or newer])])
dnl }}}

dnl {{{ check for libmagic support
AC_MSG_CHECKING([whether to enable libmagic support])
AC_ARG_ENABLE([libmagic],
			  AS_HELP_STRING([--enable-libmagic], [Enable libmagic support]),
			  [enable_magic=$enableval],
			  [enable_magic=no])
AC_MSG_RESULT([$enable_magic])

AM_CONDITIONAL(MAGIC, test x"$enable_magic" = x"yes")
if test x"$enable_magic" = x"yes"; then
	AC_CHECK_LIB(magic, magic_open)
	AC_CHECK_HEADERS(magic.h,,
					 [AC_MSG_ERROR([magic.h is required for libmagic support])])
	AC_DEFINE([ENABLE_MAGIC], [1], [Enable libmagic support])
fi
dnl }}}

dnl {{{ check for Lua support
AC_MSG_CHECKING([whether to enable Lua scripting support])
AC_ARG_ENABLE([lua],
			  AS_HELP_STRING([--enable-lua], [Enable Lua scripting support]),
			  [enable_lua=$enableval],
			  [enable_lua=no])
AC_MSG_RESULT([$enable_lua])

AM_CONDITIONAL(LUA, test x"$enable_lua" = x"yes")
if test x"$enable_lua" = x"yes"; then
	PKG_CHECK_MODULES([lua], [lua >= 5.1],,
					  [AC_MSG_ERROR([Lua support requires lua-5.1 or newer])])
	AC_DEFINE([ENABLE_LUA], [1], [Enable Lua support])
fi
dnl }}}

dnl {{{ extra cflags for gcc
ENVTAG_CXXFLAGS=
if test x"$GCC" = x"yes"; then
	ENVTAG_CXXFLAGS="$ENVTAG_CXXFLAGS -Wall"
fi
AC_SUBST([ENVTAG_CXXFLAGS])
dnl }}}

dnl {{{ output
AM_CONFIG_HEADER(config.h)
AC_OUTPUT(
		  Makefile
		  src/Makefile
		  examples/Makefile
		  examples/lua/Makefile
		  )
dnl }}}
