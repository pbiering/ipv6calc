#!/bin/sh
#
# Project    : ipv6calc
# File       : test_ipv6logstats.sh
# Version    : $Id: test_ipv6logstats.sh,v 1.1 2003/06/15 12:17:49 peter Exp $
# Copyright  : 2003 by Peter Bieringer <pb (at) bieringer.de>
#
# Test program for "ipv6logstats"

echo "currently broken!!"
#exit 1

testscenarios() {
# Requests
cat <<END | grep -v "^#"
2002:d9e4:ef87::1 - - [09/Jun/2003:10:16:56 +0200] "GET /stats/result.png HTTP/1.0" 200 4244 "-" "Privoxy/3.0 (Anonymous)"
3ffe:ffff:1:2:3:4:5:6 - - [15/Jun/2003:05:01:56 +0200] "GET /favicon.ico HTTP/1.1" 200 1011 "http://www.bieringer.de/linux/IPv6/" "Privoxy/3.0 (Anonymous)"
192.168.1.1 - - [09/Jun/2003:10:16:56 +0200] "GET /stats/result.png HTTP/1.0" 200 4244 "-" "Privoxy/3.0 (Anonymous)"
END
}

#set -x
## main ##
echo "Run 'ipv6logstats' function tests..."

testscenarios
#testscenarios | ./ipv6logstats
retval=$?
if [ $retval -ne 0 ]; then
	echo "Error executing 'ipv6logstats'!"
	exit 1
fi
echo 
echo "All tests were successfully done!"
