#!/bin/bash
#
# Project    : ipv6calc/ipv6calcweb
# File       : test_ipv6calcweb.sh
# Version    : $Id$
# Copyright  : 2012-2014 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Test script for ipv6calcweb
#

while getopts "Vf:d" opt; do
	case $opt in
	    f)
		opt_format="$OPTARG"
		;;
	    d)
		opt_debug=1
		;;
	    V)
		verbose=1
		;;
	    *)
		echo "$0 [-V] [-d] [-f <format>]"
		echo "    -d		debug"
		echo "    -V		verbose"
		echo "    -f <format>	format option"
		exit 1
		;;
	esac
done

shift $[ $OPTIND - 1 ]

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
test="run 'ipv6calc' very basic output format tests"
echo "INFO  : $test"
for format in textkeyvalue text html htmlfull; do
	if [ -n "$opt_format" -a "$opt_format" != "$format" ]; then
		echo "NOTICE: skip format: $format"
		continue
	fi
	[ "$verbose" = "1" ] && echo "INFO  : test format: $format"
	[ "$verbose" = "1" ] && echo "DEBUG : execute: HTTP_IPV6CALCWEB_DEBUG=0x1000 HTTP_IPV6CALCWEB_OUTPUT_FORMAT=\"$format\" ./ipv6calcweb.cgi"
	if [ "$opt_debug" = "1" ]; then
		HTTP_IPV6CALCWEB_DEBUG="0x1000" HTTP_IPV6CALCWEB_OUTPUT_FORMAT="$format" ./ipv6calcweb.cgi
		rc=$?
	else
		if [ "$verbose" = "1" ]; then
			HTTP_IPV6CALCWEB_DEBUG="0x1000" HTTP_IPV6CALCWEB_OUTPUT_FORMAT="$format" ./ipv6calcweb.cgi >/dev/null
			rc=$?
		else
			HTTP_IPV6CALCWEB_DEBUG="0x1000" HTTP_IPV6CALCWEB_OUTPUT_FORMAT="$format" ./ipv6calcweb.cgi >/dev/null 2>/dev/null
			rc=$?
		fi
	fi
	if [ $rc -ne 0 ];then
		echo "ERROR : output format reports an error: $format"
		if [ "$opt_debug" != "1" ]; then
			HTTP_IPV6CALCWEB_OUTPUT_FORMAT="$format" ./ipv6calcweb.cgi
		fi
		exit 1
	fi
	[ "$verbose" = "1" ] || echo -n "."
done || exit 1
[ "$verbose" = "1" ] || echo
echo "INFO  : $test successful"



## more sophisticated checks
REMOTE_ADDR="$1"
REMOTE_HOST="client.domain.example"
HTTP_USER_AGENT="test_ipv6calcweb"

SERVER_ADDR="192.0.2.1"
SERVER_NAME="server.domain.example"

QUERY_STRING="$2"

HTTP_IPV6CALCWEB_DEBUG="0x1000"
HTTP_IPV6CALCWEB_INFO_SERVER="1"

HTTP_X_FORWARDED_FOR="5.6.7.8, 9.10.11.12"
HTTP_VIA="1.0 fred, 1.1 nowhere.com"

export REMOTE_ADDR REMOTE_HOST HTTP_USER_AGENT SERVER_ADDR SERVER_NAME QUERY_STRING HTTP_IPV6CALCWEB_DEBUG HTTP_IPV6CALCWEB_INFO_SERVER HTTP_X_FORWARDED_FOR HTTP_VIA

test="run 'ipv6calc' sophisticated tests"
echo "INFO  : $test"
for format in textkeyvalue text html htmlfull; do
	if [ -n "$opt_format" -a "$opt_format" != "$format" ]; then
		echo "NOTICE: skip format: $format"
		continue
	fi

	HTTP_IPV6CALCWEB_OUTPUT_FORMAT="$format"
	export HTTP_IPV6CALCWEB_OUTPUT_FORMAT

	if [ "$verbose" = "1" ]; then
		OUTPUT="`./ipv6calcweb.cgi`"
		result=$?
	else
		OUTPUT="`./ipv6calcweb.cgi 2>&1`"
		result=$?
	fi
	if [ "$opt_debug" = "1" ]; then
		echo "$OUTPUT"
	fi

	if [ $result -ne 0 ];then
		echo "ERROR : output format reports an error: $format"
		if [ "$opt_debug" != "1" ]; then
			echo "$OUTPUT"
		fi
		exit 1
	fi
	[ "$verbose" = "1" ] || echo -n "."
done || exit 1
[ "$verbose" = "1" ] || echo
echo "INFO  : $test successful"

#if [ $result -ne 0 ]; then
#	echo "TEST FAILED (exit code != 0)"
#	HTTP_IPV6CALCWEB_DEBUG="0xffff"
#	export HTTP_IPV6CALCWEB_DEBUG
#	./ipv6calcweb.cgi
#	exit 1
#else
#	# check output
#	if echo "$OUTPUT" | egrep -q "(ERROR|problem)"; then
#		echo "TEST FAILED (ERROR|problem) occurs"
#		HTTP_IPV6CALCWEB_DEBUG="0xffff"
#		export HTTP_IPV6CALCWEB_DEBUG
#		./ipv6calcweb.cgi
#		exit 1
#	fi
#	if echo "$OUTPUT" | egrep -q "(reserved)"; then
#		[ "$verbose" = "1" ] || echo "$OUTPUT"
#	fi
#fi
