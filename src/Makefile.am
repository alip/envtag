SUBDIRS = scripts .

AM_CXXFLAGS= $(taglib_CFLAGS) $(lua_CFLAGS) @ENVTAG_CXXFLAGS@ \
	     -DLIBEXECDIR=\"$(libexecdir)\" -DGITHEAD=\"$(GITHEAD)\"
bin_PROGRAMS= envtag
envtag_SOURCES= defs.hh script.hh script.cpp util.hh util.cpp main.cpp
envtag_LDADD= $(taglib_LIBS) $(lua_LIBS)

if MAGIC
envtag_SOURCES+= mime.hh mime.cpp
endif
