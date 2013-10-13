#!/bin/sh
#
# Project    : ipv6calc
# File       : autogen-all-variants.sh
# Version    : $Id: autogen-all-variants.sh,v 1.11 2013/10/13 16:18:44 ds6peter Exp $
# Copyright  : 2011-2013 by Peter Bieringer <pb (at) bieringer.de>
#
# Information: run autogen.sh with all supported variants

autgen_variants() {
	cat <<END
-i
-i --ip2location-dyn
-g
-g --geoip-dyn
-g --geoip-ipv6-compat
-g --geoip-ipv6-compat --geoip-dyn
-a
--disable-db-ieee
--disable-db-ipv4
--disable-db-ipv6
--disable-db-ipv6 --disable-db-ipv4
--disable-db-ipv6 --disable-db-ipv4 --disable-db-ieee
--disable-db-ipv6 --disable-db-ieee
--disable-db-ipv4 --disable-db-ieee
END
}

options_add="$*"

if [ -n "$options_add" ]; then
	echo "INFO  : additional options: $options_add"
fi

# basic defaults
nice -n 10 ./autogen.sh $options_add
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh (basic) $options_add' reports an error"
	exit 1
fi

# variants
autgen_variants | while read buildoptions; do
	nice -n 10 ./autogen.sh $buildoptions $options_add
	if [ $? -ne 0 ]; then
		echo "ERROR : 'autogen.sh $buildoptions $*' reports an error"
		exit 1
	fi
done || exit 1


make distclean

echo "INFO  : congratulations, all variants built successful!"

