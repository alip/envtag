AM_MAKEFLAGS= --no-print-directory
AUTOMAKE_OPTIONS= dist-bzip2 no-dist-gzip std-options subdir-objects foreign

CLEANFILES= *~
MAINTAINERCLEANFILES= \
		      Makefile.in \
		      configure \
		      aclocal.m4 \
		      config.h \
		      config.h.in \
		      INSTALL

EXTRA_DIST= README.mkd
SUBDIRS = . src examples

TAR_FILE= $(PACKAGE)-$(VERSION).tar.bz2
SHA1_FILE= $(TAR_FILE).sha1sum
GPG_FILE= $(SHA1_FILE).asc

$(SHA1_FILE): dist
	@echo "SHA1 $(TAR_FILE)"
	sha1sum $(TAR_FILE) > $(SHA1_FILE)

$(GPG_FILE): $(SHA1_FILE)
	@echo "SIGN $(SHA1_FILE)"
	gpg --detach-sign --armor $(SHA1_FILE)

.PHONY: upload-release
upload-release: $(SHA1_FILE) $(GPG_FILE)
	rsync --partial --progress -ave ssh $(TAR_FILE) $(SHA1_FILE) $(GPG_FILE) tchaikovsky.exherbo.org:public_html/envtag/
