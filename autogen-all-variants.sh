#!/bin/sh
#
# Project    : ipv6calc
# File       : autogen-all-variants.sh
# Version    : $Id: autogen-all-variants.sh,v 1.1 2011/05/26 17:30:54 peter Exp $
# Copyright  : 2011-2011 by Peter Bieringer <pb (at) bieringer.de>
#
# Information: run autogen.sh with all supported variants

./autogen.sh
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh' reports an error"
	exit 1
fi


./autogen.sh -i
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh -i' reports an error"
	exit 1
fi

./autogen.sh -g
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh -g' reports an error"
	exit 1
fi

./autogen.sh -a
if [ $? -ne 0 ]; then
	echo "ERROR : 'autogen.sh -a' reports an error"
	exit 1
fi

echo "INFO  : congratulations, all variants built successful!"

make distclean
