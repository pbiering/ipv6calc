#!/bin/sh

$Id: autogen.sh,v 1.10 2011/05/12 14:22:14 peter Exp $

OPTIONS_CONFIGURE=""

LAST=""
while [ "$1" != "$LAST" ]; do
	LAST="$1"
	case $1 in
	    '--no-make'|'-n')
		shift
		flag_no_make=1
		;;
	    '--all'|'-a')
		shift
		OPTIONS_CONFIGURE="--enable-geoip --enable-ip2location"
		;;
	    '--geoip'|'-g')
		shift
		OPTIONS_CONFIGURE="--enable-geoip"
		;;
	    '-?'|'-h'|'--help')
		echo "Supported options:"
		echo "   -?|-h|--help: this help"
		echo "   -n|--no-make: stop before running 'make'"
		echo "   -a|--all    : enable GeoIP and IP2Location support"
		echo "   -g|--geoip  : enable GeoIP support"
		exit 1
	esac
done

if [ -f Makefile ]; then
	echo "*** cleanup"
	make autoclean
fi

echo "*** run: autoheader"
autoheader || exit 1

echo "*** run: autoconf"
autoconf || exit 1

echo "*** run: configure, options: $OPTIONS_CONFIGURE $*"
./configure --bindir=/usr/bin --mandir=/usr/share/man $OPTIONS_CONFIGURE $* || exit 1

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

echo
echo "For installing the binaries, type: make install"

