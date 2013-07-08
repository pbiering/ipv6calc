#!/bin/sh
#
# Project    : ipv6calc/logstats
# File       : test_ipv6logstats.sh
# Version    : $Id: test_ipv6logstats.sh,v 1.7 2013/07/08 08:52:42 ds6peter Exp $
# Copyright  : 2003-2005 by Peter Bieringer <pb (at) bieringer.de>
#
# Test program for "ipv6logstats"


testscenarios() {
# Requests
cat <<END | grep -v "^#"
2002:d9e4:ef87::1 - - [09/Jun/2003:10:16:56 +0200] "GET /stats/result.png HTTP/1.0" 200 4244 "-" "Privoxy/3.0 (Anonymous)"
3ffe:ffff:1:2:3:4:5:6 - - [15/Jun/2003:05:01:56 +0200] "GET /favicon.ico HTTP/1.1" 200 1011 "http://www.bieringer.de/linux/IPv6/" "Privoxy/3.0 (Anonymous)"
3ffe:831f:ce49:7601:8000:efff:62c3:fffe - - [15/Jun/2003:05:01:56 +0200] "GET /teredo.ico HTTP/1.1" 200 1011 "http://www.bieringer.de/linux/IPv6/" "Privoxy/3.0 (Anonymous)"
3FFE:1a05:510:200:0:5EFE:8CAD:8108 - - [15/Jun/2003:05:01:56 +0200] "GET /isatap.ico HTTP/1.1" 200 1011 "http://www.bieringer.de/linux/IPv6/" "Privoxy/3.0 (Anonymous)"
2001:db8::a0fc:4941:a0fc:3041 - - [15/Jun/2003:05:01:56 +0200] "GET /iid-privacy.ico HTTP/1.1" 200 1011 "http://www.bieringer.de/linux/IPv6/" "Privoxy/3.0 (Anonymous)"
192.168.1.1 - - [09/Jun/2003:10:16:56 +0200] "GET /stats/result.png HTTP/1.0" 200 4244 "-" "Privoxy/3.0 (Anonymous)"
2001:db8::a0fc:4291:a0fc:1884 - - [15/Jun/2003:05:01:56 +0200] "GET /iid-static.ico HTTP/1.1" 200 1011 "http://www.bieringer.de/linux/IPv6/" "Privoxy/3.0 (Anonymous)"
END
}

#set -x
## version
echo "Run 'ipv6logstats' version test..."
./ipv6logstats -vvv
if [ $retval -ne 0 ]; then
	echo "Error executing 'ipv6logstats'!"
	exit 1
fi
echo 

## main ##
echo "Run 'ipv6logstats' function tests..."

#testscenarios
testscenarios | ./ipv6logstats -q
retval=$?
if [ $retval -ne 0 ]; then
	echo "Error executing 'ipv6logstats'!"
	exit 1
fi
echo 


#testscenarios (columns)
testscenarios | ./ipv6logstats -q -c
retval=$?
if [ $retval -ne 0 ]; then
	echo "Error executing 'ipv6logstats'!"
	exit 1
fi
echo


#testscenarios version 3
testscenarios | ./ipv6logstats -q | grep -q '\*3\*CC-proto-code-list/ALL'
if [ $retval -ne 0 ]; then
	echo "Error executing 'ipv6logstats' (version 3 test)"
	exit 1
fi


echo "All tests were successfully done!"
