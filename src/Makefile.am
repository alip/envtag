SUBDIRS= commands modules .
AM_CXXFLAGS= $(taglib_CFLAGS) $(lua_CFLAGS) @ENVTAG_CXXFLAGS@ \
	     -DLIBEXECDIR=\"$(libexecdir)\" -DGITHEAD=\"$(GITHEAD)\"

bin_PROGRAMS= envtag
envtag_SOURCES= envlua.hh envlua.cpp envmain.cpp
envtag_LDADD= $(taglib_LIBS) $(lua_LIBS)

if MAGIC
envtag_SOURCES+= envmime.hh envmime.cpp
endif
