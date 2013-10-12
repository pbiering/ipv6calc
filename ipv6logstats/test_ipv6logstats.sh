#!/bin/sh
#
# Project    : ipv6calc/logstats
# File       : test_ipv6logstats.sh
# Version    : $Id: test_ipv6logstats.sh,v 1.11 2013/10/12 20:55:06 ds6peter Exp $
# Copyright  : 2003-2013 by Peter Bieringer <pb (at) bieringer.de>
#
# Test program for "ipv6logstats"

if [ ! -x ./ipv6logstats ]; then
	echo "Binary './ipv6logstats' missing or not executable"
	exit 1
fi

# Test proper option definitions
echo "Test proper option definition"
./ipv6logstats -h >/dev/null
if [ $? -ne 1 ]; then
	echo "ERROR : something wrong in option definition"
	exit 1
fi


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


testscenarios_match() {
	cat <<END | grep -v "^#"
# Anonymized IPv4
246.24.59.65					ASN-num-proto/15169/IPv4
246.24.59.65					CC-proto-code/IPv4/AU
::246.24.59.65					ASN-num-proto/15169/IPv4
::246.24.59.65					CC-proto-code/IPv4/AU
::ffff:246.24.59.65				ASN-num-proto/15169/IPv4
::ffff:246.24.59.65				CC-proto-code/IPv4/AU
END
}

#set -x
## version
echo "Run 'ipv6logstats' version test..."
./ipv6logstats -vvv
retval=$?
if [ $retval -ne 0 ]; then
	echo "Error executing 'ipv6logstats'!"
	exit 1
fi
echo 

echo "Run 'ipv6logstats' version test in debug mode..."
./ipv6logstats -vvv -d -1
retval=$?
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
retval=$?
if [ $retval -ne 0 ]; then
	echo "Error executing 'ipv6logstats' (version 3 test)"
	exit 1
fi

# testscenarios matching
testscenarios_match | while read ip match; do
	echo -n "INFO  : test $ip for match $match: "
	if echo "$ip" | ./ipv6logstats -q | grep -q "$match"; then
		echo "OK"
	else
		echo "ERROR, unexpected result:"
		echo "$ip" | ./ipv6logstats -q | grep -v "DB-Info"
		exit 1
	fi
done || exit 1


echo "All tests were successfully done!"
