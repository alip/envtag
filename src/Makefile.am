AM_CFLAGS= $(taglib_c_CFLAGS) @ENVTAG_CFLAGS@ -DGITHEAD=\"$(GITHEAD)\"
bin_PROGRAMS= envtag
envtag_SOURCES= main.c
envtag_LDADD= $(taglib_c_LIBS)

if LUA
AM_CFLAGS+= $(lua_CFLAGS)
envtag_SOURCES+= script.h script.c
envtag_LDADD+= $(lua_LIBS)
endif
