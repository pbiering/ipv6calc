#!/bin/sh
#
# Project    : ipv6calc/ipv6calcweb
# File       : test_ipv6calcweb.sh
# Version    : $Id: test_ipv6calcweb.sh,v 1.11 2013/10/30 20:04:25 ds6peter Exp $
# Copyright  : 2012-2013 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Test script for ipv6calcweb
#

if [ -z "$1" ]; then
	echo "Need IPv4/v6 address as argument #1"
	exit 1
fi

if [ ! -f ipv6calcweb.cgi ]; then
	make || exit 1
fi
if [ ! -x ipv6calcweb.cgi ]; then
	chmod u+x ipv6calcweb.cgi
fi

## very basic output format tests
for format in text html htmlfull; do
	HTTP_IPV6CALCWEB_OUTPUT_FORMAT="$format" ./ipv6calcweb.cgi >/dev/null
	if [ $? -ne 0 ];then
		echo "ERROR : output format reports an error: $format"
		HTTP_IPV6CALCWEB_OUTPUT_FORMAT="$format" ./ipv6calcweb.cgi
		exit 1
	fi
done


## more sophisticated checks
REMOTE_ADDR="$1"
REMOTE_HOST="client.domain.example"
HTTP_USER_AGENT="test_ipv6calcweb"

SERVER_ADDR="192.0.2.1"
SERVER_NAME="server.domain.example"

QUERY_STRING="$2"

HTTP_IPV6CALCWEB_DEBUG="0x1000"

export REMOTE_ADDR REMOTE_HOST HTTP_USER_AGENT SERVER_ADDR SERVER_NAME QUERY_STRING HTTP_IPV6CALCWEB_DEBUG

OUTPUT="`./ipv6calcweb.cgi`"

result=$?
echo "Result: $result"

if [ $result -ne 0 ]; then
	echo "TEST FAILED (exit code != 0)"
	HTTP_IPV6CALCWEB_DEBUG="0xffff"
	export HTTP_IPV6CALCWEB_DEBUG
	./ipv6calcweb.cgi
	exit 1
else
	# check output
	if echo "$OUTPUT" | egrep -q "(ERROR|problem)"; then
		echo "TEST FAILED (ERROR|problem) occurs"
		HTTP_IPV6CALCWEB_DEBUG="0xffff"
		export HTTP_IPV6CALCWEB_DEBUG
		./ipv6calcweb.cgi
		exit 1
	fi
	if echo "$OUTPUT" | egrep -q "(reserved)"; then
		echo "$OUTPUT"
	fi
fi
