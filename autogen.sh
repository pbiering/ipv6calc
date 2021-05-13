#!/usr/bin/env bash
#
# Project    : ipv6calc
# File       : autogen.sh
# Version    : $Id$
# Copyright  : 2003-2021 by Peter Bieringer <pb (at) bieringer.de>
#
# Information: autogeneration of projects with optional features

OPTIONS_CONFIGURE=""

flag_no_make=false
use_ip2location=false
use_ip2location_dyn=false
RELAX=false
SKIP_STATIC=false
SKIP_TEST=false
USE_CLANG=false

LAST=""
while [ "$1" != "$LAST" ]; do
	LAST="$1"
	case $1 in
	    '--no-make'|'-n')
		shift
		flag_no_make=true
		;;
	    '--all'|'-a')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-ip2location --enable-mmdb --enable-external --enable-mod_ipv6calc"
		SKIP_STATIC=true
		use_ip2location=true
		;;
	    '--ALL'|'-A')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-ip2location --enable-mmdb --with-ip2location-dynamic --with-mmdb-dynamic --enable-external --enable-mod_ipv6calc"
		SKIP_STATIC=true
		use_ip2location=true
		;;
	    '--mmdb'|'-m')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-mmdb"
		SKIP_STATIC=true
		;;
	    '--mmdb-dyn'|'-M')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-mmdb --with-mmdb-dynamic"
		SKIP_STATIC=true
		;;
	    '--ip2location'|'-i')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-ip2location"
		SKIP_STATIC=true
		use_ip2location=true
		;;
	    '--ip2location-dyn'|'-I')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-ip2location --with-ip2location-dynamic"
		SKIP_STATIC=true
		use_ip2location=true
		use_ip2location_dyn=true
		;;
	    '--external'|'-e')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-external"
		SKIP_STATIC=true
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
	    '--disable-builtin')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --disable-db-ipv6 --disable-db-ipv4 --disable-db-ieee --disable-db-ipv4  --disable-db-ipv6  --disable-db-as-registry --disable-db-cc-registry"
		;;
	    '-S')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-shared"
		SKIP_STATIC=true
		;;
	    '--no-static-build')
		shift
		SKIP_STATIC=true
		;;
	    '--no-test')
		shift
		SKIP_TEST=true
		;;
	    '--clang')
		shift
		USE_CLANG=true
		;;
	    '--gcc-Os')
		shift
		CFLAGS="$CFLAGS -Os"
		;;
	    '--m32')
		shift
		OPTIONS_CONFIGURE="${OPTIONS_CONFIGURE:+$OPTIONS_CONFIGURE }--enable-m32"
		;;
	    '--relax')
		shift
		RELAX=true
		;;
	    '-?'|'-h'|'--help')
		echo "Supported options:"
		echo "   -?|-h|--help        : this help"
		echo "   -n|--no-make        : stop before running 'make'"
		echo "   -a|--all            : enable GeoIP/IP2Location/db-ip.com/External/mod_ipv6calc support"
		echo "   -A|--ALL            : enable GeoIP/IP2Location/db-ip.com/External/mod_ipv6calc support with dynamic library support"
		echo "   -m|--mmdb           : enable MaxMindDB support (GeoIP/db-ip.com)"
		echo "   --mmdb-dyn|-M       : switch to dynamic library loading of MaxMindDB"
		echo "   --disable-geoip2    : disable MaxMindDB support for GeoIP"
		echo "   --disable-dbip2     : disable MaxMindDB support for db-ip.com"
		echo "   -i|--ip2location    : enable IP2Location support"
		echo "   --ip2location-dyn|-I: switch to dynamic library loading of IP2Location"
		echo "   -e|--external       : enable external database support"
		echo "   --disable-db-ieee   : disable built-in IEEE database"
		echo "   --disable-db-ipv4   : disable built-in IPv4 database"
		echo "   --disable-db-ipv6   : disable built-in IPv6 database"
		echo "   --disable-builtin   : disable all internal databases"
		echo "   -S                  : enable shared library mode"
		echo "   --no-static-build   : skip static build"
		echo "   --no-test           : skip 'make test'"
		echo "   --clang             : use 'clang' instead of default (usually 'gcc')"
		echo "   --gcc-Os            : use 'gcc' option '-Os'"
		echo "   --m32               : compile for 32-bit"
		echo "   --relax             : don't stop on compiler warnings"
		exit 1
		;;
	    *)
		if [ -n "$1" ]; then
			OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE $1"
			shift
		fi
		;;
	esac
done

source ./autogen-support.sh "source"

if $use_ip2location; then
	if ! echo "$OPTIONS_CONFIGURE" | grep -q 'with-ip2location-headers='; then
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE"
	fi
fi

if $RELAX; then
	if ! echo "$OPTIONS_CONFIGURE" | grep -q 'disable-compiler-warning-to-error'; then
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --disable-compiler-warning-to-error"
	fi
fi

if [ -f Makefile ]; then
	echo "*** cleanup"
	$MAKE autoclean
fi

if $USE_CLANG; then
	if [ -x "/usr/bin/clang" ]; then
		export CC=/usr/bin/clang
	else
		echo "ERROR : --clang selected but binary is missing: /usr/bin/clang"
		exit 1
	fi

	case "$OSTYPE" in
	    freebsd*)
		# clang requires hint to /usr/local
		LDFLAGS=-L/usr/local/lib CFLAGS=-I/usr/local/include
		;;
	esac
fi

echo "*** run: autoheader"
autoheader || exit 1

echo "*** run: autoconf"
autoconf || exit 1

echo "*** run: configure, options: $OPTIONS_CONFIGURE $*"
CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS -Wl,--as-needed" ./configure --bindir=/usr/bin --mandir=/usr/share/man $OPTIONS_CONFIGURE $* || exit 1

if $flag_no_make; then
	echo
	echo "Stop before running 'make'"
	exit
fi

echo "*** run: make clean"
$MAKE clean || exit 1

if $use_ip2location_dyn; then
	if ldd ./ipv6calc/ipv6calc | grep -i IPLocation; then
		echo "ERROR : dynamic library mode enabled, but ldd of binary still reports reference to IP2Location"
		exit 1
	fi
fi

if [ "$use_geoip_dyn" = "1" ]; then
	if ldd ./ipv6calc/ipv6calc | grep -i GeoIP; then
		echo "ERROR : dynamic library mode enabled, but ldd of binary still reports reference to GeoIP"
		exit 1
	fi
fi

$MAKE
if [ $? -ne 0 ]; then
	echo "ERROR : 'make' was not successful with configure options: $OPTIONS_CONFIGURE"
	exit 1
fi

if $SKIP_TEST; then
	echo "*** skip: make test"
else
	echo "*** run: make test"
	$MAKE test
	if [ $? -ne 0 ]; then
		echo "ERROR : 'make test' was not successful with configure options: $OPTIONS_CONFIGURE"
		exit 1
	fi
fi

if ! $SKIP_STATIC && ! [ -e /etc/redhat-release ]; then
	# skip static on non Fedora/RedHat/CentOS systems
	SKIP_STATIC=true
	echo "NOTICE: 'make static' skipped on non Fedora/RedHat/CentOS systems"
fi

if ! $SKIP_STATIC; then
	echo "*** run: make static"
	$MAKE static
	if [ $? -ne 0 ]; then
		echo "ERROR : 'make static' reports an error (perhaps glibc-static/openssl-static/zlib-static is missing)"
		exit 1
	fi
fi

echo "*** run: make -n install (dummy install test)"
$MAKE -n install
if [ $? -ne 0 ]; then
        echo "ERROR : 'make -n install' reports an error"
        exit 1
fi

echo
echo "For installing the binaries, type: make install"
