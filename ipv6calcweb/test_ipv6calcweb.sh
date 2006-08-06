#!/bin/sh

if [ -z "$1" ]; then
	echo "Need IPv4/v6 address as argument #1"
	exit 1
fi

if [ ipv6calcweb.cgi.in -nt ipv6calcweb.cgi ]; then
	cat ipv6calcweb.cgi.in | sed s/\@PACKAGE_VERSION\@/test/ >ipv6calcweb.cgi
	chmod u+x ipv6calcweb.cgi
fi

REMOTE_ADDR="$1"
REMOTE_HOST="client.domain.example"
HTTP_USER_AGENT="test_ipv6calcweb"

SERVER_ADDR="192.0.2.1"
SERVER_NAME="server.domain.example"

QUERY_STRING="$2"

export REMOTE_ADDR REMOTE_HOST HTTP_USER_AGENT SERVER_ADDR SERVER_NAME QUERY_STRING

./ipv6calcweb.cgi
