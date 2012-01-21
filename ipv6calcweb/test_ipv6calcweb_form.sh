#!/bin/sh
#
# Project    : ipv6calc/ipv6calcweb
# File       : test_ipv6calcweb_form.sh
# Version    : $Id: test_ipv6calcweb_form.sh,v 1.2 2012/01/21 18:55:35 peter Exp $
# Copyright  : 2012-2012 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Test script for form mode of ipv6calcweb
#


# Test Scenarios for autodetection "good case"
source ../ipv6calc/test_scenarios.sh


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

REMOTE_ADDR="127.0.0.1"
REMOTE_HOST="client.domain.example"
HTTP_USER_AGENT="test_ipv6calcweb"

SERVER_ADDR="192.0.2.1"
SERVER_NAME="server.domain.example"

HTTP_IPV6CALCWEB_MODE="form"
HTTP_IPV6CALCWEB_DEBUG="0x1000"

export REMOTE_ADDR REMOTE_HOST HTTP_USER_AGENT SERVER_ADDR SERVER_NAME QUERY_STRING HTTP_IPV6CALCWEB_MODE HTTP_IPV6CALCWEB_DEBUG

# extract tokenhash & tokentime
OUTPUT="`./ipv6calcweb.cgi`"
result=$?
if [ $result -ne 0 ]; then
	echo "TEST FAILED"
	exit 1
else
	# check output
	if echo "$OUTPUT" | egrep -q "(ERROR|problem)"; then
		echo "TEST FAILED"
		exit 1
	fi
fi 

tokenhash="`echo "$OUTPUT" | grep 'name="tokenhash"' | sed 's/.* name="tokenhash" value="\([0-9a-f]*\)" .*/\1/'`"
tokentime="`echo "$OUTPUT" | grep 'name="tokentime"' | sed 's/.* name="tokentime" value="\([0-9]*\)" .*/\1/'`"
if [ ${#tokenhash} -ne 72 ]; then
	echo "TEST FAILED (tokenhash extraction)"
	echo "$OUTPUT"
	exit 1
fi
if [ ${#tokentime} -ne 10 ]; then
	echo "TEST FAILED (tokentime extraction)"
	echo "$OUTPUT"
	exit 1
fi

echo "DEBUG: extracted tokenhash: $tokenhash"
echo "DEBUG: extracted tokentime: $tokentime"

testscenarios_auto_good | grep -v "^#" | egrep -vw "(bitstring|base85)" | grep -v "%" | while read input type; do
	input_escaped="$(perl -MURI::Escape -e 'print uri_escape($ARGV[0]);' "$input")"
	QUERY_STRING="input=$input_escaped&tokenhash=$tokenhash&tokentime=$tokentime"

	echo "Test: $input ($input_escaped) ($type)"
	echo "QUERY_STRING=$QUERY_STRING"

	export REMOTE_ADDR REMOTE_HOST HTTP_USER_AGENT SERVER_ADDR SERVER_NAME QUERY_STRING HTTP_IPV6CALCWEB_MODE

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
	fi
done || exit $?

