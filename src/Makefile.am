SUBDIRS= commands modules .
DEFS+= -DLIBEXECDIR=\"$(libexecdir)\" -DGITHEAD=\"$(GIT_HEAD)\"
AM_CXXFLAGS= $(taglib_CFLAGS) $(lua_CFLAGS) @ENVTAG_CXXFLAGS@

bin_PROGRAMS= envtag
envtag_SOURCES= envlua.hh envlua.cpp envmain.cpp
envtag_LDADD= $(taglib_LIBS) $(lua_LIBS)

if MAGIC
envtag_SOURCES+= envmime.hh envmime.cpp
endif
