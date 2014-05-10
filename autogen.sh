#!/bin/sh
#
# Project    : ipv6calc
# File       : autogen.sh
# Version    : $Id: autogen.sh,v 1.37 2014/05/10 12:34:32 ds6peter Exp $
# Copyright  : 2003-2014 by Peter Bieringer <pb (at) bieringer.de>
#
# Information: autogeneration of projects with optional features

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
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-geoip --enable-ip2location"
		SKIP_STATIC=1
		;;
	    '--geoip'|'-g')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-geoip"
		SKIP_STATIC=1
		;;
	    '--geoip-dyn'|'-G')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-geoip --with-geoip-dynamic"
		SKIP_STATIC=1
		;;
	    '--ip2location'|'-i')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-ip2location"
		SKIP_STATIC=1
		;;
	    '--ip2location-dyn'|'-I')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-ip2location --with-ip2location-dynamic"
		SKIP_STATIC=1
		;;
	    '--disable-db-ieee')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --disable-db-ieee"
		;;
	    '--disable-db-ipv4')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --disable-db-ipv4"
		;;
	    '--disable-db-ipv6')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --disable-db-ipv6"
		;;
	    '--geoip-ipv6-compat')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --with-geoip-ipv6-compat"
		SKIP_STATIC=1
		;;
	    '-S')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-shared"
		SKIP_STATIC=1
		;;
	    '--no-static-build')
		shift
		SKIP_STATIC=1
		;;
	    '--enable-bundled-md5'|'--enable-bundled-getopt')
		# whitelisted
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE $1"
		shift
		;;
	    '-W')
		shift
		EXTRA_CFLAGS="-Wformat"
		gcc_date=$(gcc -v 2>&1 | grep -i version | sed 's/.*\s\([0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]\)\s.*/\1/g')
		if [ -n "$gcc_date" ]; then
			if [ $gcc_date -gt 20080704 ]; then
				# > CentOS/RHEL 5
				EXTRA_CFLAGS="$EXTRA_CFLAGS -Werror -Werror=format-security"
			fi
		fi
		echo "INFO  : option -W enables EXTRA_CFLAGS=$EXTRA_CFLAGS"
		;;
	    '-?'|'-h'|'--help')
		echo "Supported options:"
		echo "   -?|-h|--help        : this help"
		echo "   -W                  : use extra CFLAGS=-Werror"
		echo "   -n|--no-make        : stop before running 'make'"
		echo "   -a|--all            : enable GeoIP and IP2Location support"
		echo "   -g|--geoip          : enable GeoIP support"
		echo "   --geoip-dyn|-G      : switch to dynamic library loading of GeoIP"
		echo "   -i|--ip2location    : enable IP2Location support"
		echo "   --ip2location-dyn|-I: switch to dynamic library loading of IP2Location"
		echo "   --disable-db-ieee   : disable built-in IEEE database"
		echo "   --disable-db-ipv4   : disable built-in IPv4 database"
		echo "   --disable-db-ipv6   : disable built-in IPv6 database"
		echo "   --geoip-ipv6-compat : enable GeoIP IPv6 compatibility mode"
		echo "   -S                  : enable shared library mode"
		echo "   -no-static-build    : skip static build"
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
LDFLAGS="$LDFLAGS -Wl,--as-needed" ./configure --bindir=/usr/bin --mandir=/usr/share/man $OPTIONS_CONFIGURE $* || exit 1

if [ "$flag_no_make" = "1" ]; then
	echo
	echo "Stop before running 'make'"
	exit
fi

echo "*** run: make clean"
make clean || exit 1

export EXTRA_CFLAGS

echo "*** run: make test"
make test || exit 1

if [ "$SKIP_STATIC" != "1" ]; then
	echo "*** run: make static"
	make static
	if [ $? -ne 0 ]; then
		echo "ERROR : 'make static' reports an error (perhaps glibc-static/openssl-static/zlib-static is missing)"
		exit 1
	fi
fi

echo "*** run: make -n install (dummy install test)"
make -n install
if [ $? -ne 0 ]; then
        echo "ERROR : 'make -n install' reports an error"
        exit 1
fi

echo
echo "For installing the binaries, type: make install"
