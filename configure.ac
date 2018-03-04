dnl vim: set sw=4 sts=4 ts=4 noet ft=config foldmethod=marker foldmarker={{{,}}} :

dnl {{{ program, version
AC_PREREQ(2.59)

m4_define([envtag_version_major],  [0])
m4_define([envtag_version_minor],  [5])
m4_define([envtag_version_full],   [envtag_version_major.envtag_version_minor])
m4_define([envtag_version],        [envtag_version_full])

AC_INIT([envtag], [envtag_version], [alip@exherbo.org], [envtag])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/envmain.cpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

VERSION_MAJOR=envtag_version_major
VERSION_MINOR=envtag_version_minor
VERSION_FULL="$VERSION_MAJOR.$VERSION_MINOR"
VERSION="$VERSION_FULL"

AC_SUBST([VERSION_MAJOR])
AC_SUBST([VERSION_MINOR])
AC_SUBST([VERSION_FULL])

AM_INIT_AUTOMAKE([dist-bzip2 no-dist-gzip std-options subdir-objects foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl {{{ git revision
AC_MSG_CHECKING([for git head])
if test -d "${GIT_DIR:-${ac_top_srcdir:-./}/.git}" ; then
	GIT_HEAD=`git describe 2>/dev/null`
	if test -z ${GIT_HEAD} ; then
		GIT_HEAD=`git rev-parse --short HEAD`
	fi
	if test -n "`git diff-index -m --name-only HEAD`" ; then
		GIT_HEAD=${GIT_HEAD}-dirty
	fi
	if test -n "${GIT_HEAD}" ; then
		GIT_HEAD="-${GIT_HEAD}"
	fi
fi
AC_MSG_RESULT([$GIT_HEAD])
AC_SUBST([GIT_HEAD])
dnl }}}
dnl }}}

dnl {{{ toolchain checks
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
dnl }}}

dnl {{{ Make pkg-config work
PKG_PROG_PKG_CONFIG([0.9.0])
dnl }}}

dnl {{{ check for taglib
PKG_CHECK_MODULES([taglib], [taglib >= 1.5],,
				  [AC_MSG_ERROR([envtag requires taglib-1.5 or newer])])
dnl }}}

dnl {{{ check for Lua
AC_MSG_CHECKING([lua version to build with])
AC_ARG_WITH([lua],
			  AS_HELP_STRING([--with-lua],
					         [Choose Lua version to build with (one of: lua, lua5.3, lua5.2, lua5.1, or luajit, default: auto, pick first available)]),
			  [with_lua=$withval],
			  [with_lua=auto])
AC_MSG_RESULT([$with_lua])

case "$with_lua" in
auto)
	PKG_CHECK_MODULES(lua, lua >= 5.1,,[
		PKG_CHECK_MODULES(lua, lua5.3,,[
			PKG_CHECK_MODULES(lua, lua5.2,,[
				PKG_CHECK_MODULES(lua, lua5.1,,[
					PKG_CHECK_MODULES(lua, luajit,,[
						AC_MSG_ERROR([Package requirements (lua) were not met:

No package 'lua5.3', 'lua5.2', 'lua5.1' or 'luajit' found.

Consider adjusting the PKG_CONFIG_PATH environment variable if you
installed software in a non-standard prefix.

Alternatively, you may set the environment variables LUA_CFLAGS
and LUA_LIBS to avoid the need to call pkg-config.
See the pkg-config man page for more details.])
					])
				])
			])
		])
	])
	;;
lua5.[123]|luajit)
	PKG_CHECK_MODULES(lua, $with_lua,,[
		AC_MSG_ERROR([Package requirements (lua) were not met:

No package '$with_lua' found.

Consider using --with-lua=auto if you have another version of Lua
installed.

Consider adjusting the PKG_CONFIG_PATH environment variable if you
installed software in a non-standard prefix.

Alternatively, you may set the environment variables LUA_CFLAGS
and LUA_LIBS to avoid the need to call pkg-config.
See the pkg-config man page for more details.])
	])
	;;
*)
	AC_MSG_ERROR([Invalid value for --with-lua, expected exactly one of lua5.3, lua5.2, lua5.1 or luajit])
	;;
esac
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

dnl {{{ extra cflags for gcc
ENVTAG_CXXFLAGS=
if test x"$GCC" = x"yes"; then
	ENVTAG_CXXFLAGS="$ENVTAG_CXXFLAGS -Wall"
fi
AC_SUBST([ENVTAG_CXXFLAGS])
dnl }}}

dnl {{{ output
AC_OUTPUT(
		  Makefile
		  src/Makefile
		  src/commands/Makefile
		  src/modules/Makefile
		  examples/Makefile
		  )
dnl }}}
