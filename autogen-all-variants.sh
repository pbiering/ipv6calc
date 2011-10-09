#!/bin/sh
#
# Project    : ipv6calc
# File       : autogen-all-variants.sh
# Version    : $Id: autogen-all-variants.sh,v 1.5 2011/10/09 07:17:09 peter Exp $
# Copyright  : 2011-2011 by Peter Bieringer <pb (at) bieringer.de>
#
# Information: run autogen.sh with all supported variants

./autogen.sh
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh' reports an error"
	exit 1
fi


./autogen.sh -i -d
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh -i' reports an error"
	exit 1
fi

./autogen.sh -g -d
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh -g' reports an error"
	exit 1
fi

./autogen.sh -g -d --geoip-ipv6-compat
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh -g --geoip-ipv6-compat' reports an error"
	exit 1
fi

./autogen.sh -a -d
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh -a' reports an error"
	exit 1
fi

./autogen.sh --disable-db-ieee
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh -disabe-db-ieee' reports an error"
	exit 1
fi

./autogen.sh --disable-db-ipv4
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh -disabe-db-ipv4' reports an error"
	exit 1
fi

./autogen.sh --disable-db-ipv6
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh -disabe-db-ipv6' reports an error"
	exit 1
fi

make distclean

echo "INFO  : congratulations, all variants built successful!"

