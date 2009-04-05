AM_CXXFLAGS= $(taglib_CFLAGS) @ENVTAG_CXXFLAGS@ -DGITHEAD=\"$(GITHEAD)\"
bin_PROGRAMS= envtag
envtag_SOURCES= defs.hh util.hh util.cpp main.cpp
envtag_LDADD= $(taglib_LIBS)

if MAGIC
envtag_SOURCES+= mime.hh mime.cpp
endif

if LUA
AM_CXXFLAGS+= $(lua_CFLAGS)
envtag_SOURCES+= script.hh script.cpp
envtag_LDADD+= $(lua_LIBS)
endif
