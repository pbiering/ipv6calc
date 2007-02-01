#!/bin/sh

$Id: autogen.sh,v 1.7 2007/02/01 14:41:40 peter Exp $

if [ "$1" = "--no-make" ]; then
	shift
	flag_no_make=1
fi

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

if [ "$flag_no_make" = "1" ]; then
	echo
	echo "Stop before running 'make'"
	exit
fi

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

