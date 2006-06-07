#!/bin/sh

$Id: autogen.sh,v 1.5 2006/06/07 19:43:23 peter Exp $

if [ -f Makefile ]; then
	echo "*** cleanup"
	make autoclean
fi

echo "*** run: autoconf"
autoconf || exit 1

echo "*** run: configure"
./configure --bindir=/usr/bin --mandir=/usr/share/man $* || exit 1

echo "*** run: make clean"
make clean || exit 1

echo "*** run: make"
make || exit 1

echo "*** run: make test"
make test || exit 1

echo "*** run: make -n installonly (dry-run)"
make -n installonly || exit 1

echo
echo "For installing the binaries, type: make installonly"

