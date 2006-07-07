#!/bin/sh

$Id: autogen.sh,v 1.6 2006/07/07 15:11:02 peter Exp $

if [ -f Makefile ]; then
	echo "*** cleanup"
	make autoclean
fi

echo "*** run: autoheader"
autoheader || exit 1

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

