#!/bin/sh -ex
# vim: set sw=4 et sts=4 tw=80 :

rm -fr autom4te.cache build-aux config.cache
mkdir -p m4 build-aux
aclocal
autoheader
autoconf
automake --add-missing --copy
