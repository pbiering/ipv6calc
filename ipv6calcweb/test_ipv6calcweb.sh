#!/bin/sh
#
# Project    : ipv6calc/ipv6calcweb
# File       : test_ipv6calcweb.sh
# Version    : $Id: test_ipv6calcweb.sh,v 1.7 2012/02/05 09:25:09 peter Exp $
# Copyright  : 2012-2012 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Test script for ipv6calcweb
#

if [ -z "$1" ]; then
	echo "Need IPv4/v6 address as argument #1"
	exit 1
fi

if [ ipv6calcweb.cgi.in -nt ipv6calcweb.cgi ]; then
	cp ipv6calcweb.cgi.in ipv6calcweb.cgi || exit 1
fi

# replace placeholders
version="`cat ../config.h | grep -w PACKAGE_VERSION | awk '{ print $3 }' | sed 's/"//g'`"
copyrightyear="`cat ../config.h | grep -w COPYRIGHT_YEAR | awk '{ print $3 }' | sed 's/"//g'`"
perl -pi -e "s/\@PACKAGE_VERSION\@/$version/" ipv6calcweb.cgi
perl -pi -e "s/\@COPYRIGHT_YEAR\@/$version/" ipv6calcweb.cgi

if [ ! -x ipv6calcweb.cgi ]; then
	chmod u+x ipv6calcweb.cgi
fi

REMOTE_ADDR="$1"
REMOTE_HOST="client.domain.example"
HTTP_USER_AGENT="test_ipv6calcweb"

SERVER_ADDR="192.0.2.1"
SERVER_NAME="server.domain.example"

QUERY_STRING="$2"

export REMOTE_ADDR REMOTE_HOST HTTP_USER_AGENT SERVER_ADDR SERVER_NAME QUERY_STRING

OUTPUT="`./ipv6calcweb.cgi`"

result=$?
echo "Result: $result"

if [ $result -ne 0 ]; then
	echo "TEST FAILED"
	exit 1
else
	# check output
	if echo "$OUTPUT" | egrep -q "(ERROR|problem)"; then
		echo "TEST FAILED"
		exit 1
	fi
	if echo "$OUTPUT" | egrep -q "(reserved)"; then
		echo "$OUTPUT"
	fi
fi
