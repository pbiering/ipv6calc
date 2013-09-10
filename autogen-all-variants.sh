#!/bin/sh
#
# Project    : ipv6calc
# File       : autogen-all-variants.sh
# Version    : $Id: autogen-all-variants.sh,v 1.7 2013/09/10 20:25:50 ds6peter Exp $
# Copyright  : 2011-2013 by Peter Bieringer <pb (at) bieringer.de>
#
# Information: run autogen.sh with all supported variants

autgen_variants() {
	cat <<END
-i -d
-g -d
-g --geoip-dyn -d
-g -d --geoip-ipv6-compat
-g -d --geoip-ipv6-compat --geoip-dyn
-a -d
--disable-db-ieee
--disable-db-ipv4
--disable-db-ipv6
--disable-db-ipv6 --disable-db-ipv4
--disable-db-ipv6 --disable-db-ipv4 --disable-db-ieee
--disable-db-ipv6 --disable-db-ieee
--disable-db-ipv4 --disable-db-ieee
END
}

# basic defaults
./autogen.sh
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh' reports an error"
	exit 1
fi

# variants
autgen_variants | while read buildoptions; do
	./autogen.sh $buildoptions
	if [ $? -ne 0 ]; then
		echo "ERROR : 'autogen.sh $buildoptions' reports an error"
		exit 1
	fi
done || exit 1


make distclean

echo "INFO  : congratulations, all variants built successful!"

