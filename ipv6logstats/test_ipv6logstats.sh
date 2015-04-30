#!/bin/sh
#
# Project    : ipv6calc/logstats
# File       : test_ipv6logstats.sh
# Version    : $Id: test_ipv6logstats.sh,v 1.20 2015/04/30 19:05:22 ds6peter Exp $
# Copyright  : 2003-2015 by Peter Bieringer <pb (at) bieringer.de>
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

echo "Run 'ipv6logstat' version help test for feature tokens..."
./ipv6logstats -v -h
retval=$?
if [ $retval -ne 0 ]; then
        echo "Error executing 'ipv6logstats -v -h'!"
        exit 1
fi
echo 

source ../ipv6calc/test_scenarios.sh


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
# Non-Anonymized IPv4
2.2.3.4						AS-num-proto/3215/IPv4
2.2.3.4						CC-proto-code/IPv4/FR
::2.2.3.4					AS-num-proto/3215/IPv4
::2.2.3.4					CC-proto-code/IPv4/FR
::ffff:2.2.3.4					AS-num-proto/3215/IPv4
::ffff:2.2.3.4					CC-proto-code/IPv4/FR
# Anonymized IPv4
253.134.12.143					AS-num-proto/3215/IPv4
253.134.12.143					CC-proto-code/IPv4/FR
::253.134.12.143				AS-num-proto/3215/IPv4
::253.134.12.143				CC-proto-code/IPv4/FR
::ffff:253.134.12.143				AS-num-proto/3215/IPv4
::ffff:253.134.12.143				CC-proto-code/IPv4/FR
# Teredo
3ffe:831f:ce49:7601:8000:efff:af4a:86BF		CC-code-proto/IT/IPv4
3ffe:831f:ce49:7601:8000:efff:af4a:86BF		AS-num-proto/3269/IPv4
3ffe:831f:ce49:7601:8000:efff:af4a:86BF		IPv6/Teredo/RIPE
# Teredo (anonymized)
3ffe:831f:ce49:7601:8000:ffff:a0b:f33a		CC-code-proto/IT/IPv4
3ffe:831f:ce49:7601:8000:ffff:a0b:f33a		AS-num-proto/3269/IPv4
3ffe:831f:ce49:7601:8000:ffff:a0b:f33a		IPv6/Teredo/RIPE
# 6to4
2002:c0a8:f900:9:a929:4291:4021:132d		IPv6/6to4/RESERVED
2002:c0a8:f900:9:a929:4291:4021:132d		CC-code-proto/unknown/IPv4
2002:0202:0304:0:ed08:d22b:6c15:3401		CC-proto-code/IPv4/FR
2002:0202:0304:0:ed08:d22b:6c15:3401		AS-num-proto/3215/IPv4
# 6to4 (anonymized)
2002:f618:3b41:9:a929:4941::c			CC-proto-code/IPv4/AU
2002:f618:3b41:9:a929:4941::c			AS-num-proto/15169/IPv4
# NAT64
64:ff9b::0202:0304				CC-proto-code/IPv4/FR
64:ff9b::0202:0304				AS-num-proto/3215/IPv4
64:ff9b::0202:0304				IPv6/NAT64/RIPE
# NAT64 (anonmyized)
64:ff9b::f618:3b41				CC-proto-code/IPv4/AU
64:ff9b::f618:3b41				AS-num-proto/15169/IPv4
64:ff9b::f618:3b41				IPv6/NAT64/APNIC
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

## check basic support
if ./ipv6logstats -v 2>&1| grep -qw "STAT_REG"; then
	true
else
	echo "NOTICE: SKIP ipv6logstats tests because of missing feature STAT_REG"
	exit 0
fi

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
if ./ipv6logstats -v 2>&1 | grep -qw "STAT_CC"; then
	testscenarios | ./ipv6logstats -q | grep -q '\*3\*CC-proto-code-list/ALL'
	retval=$?
	if [ $retval -ne 0 ]; then
		echo "Error executing 'ipv6logstats' (version 3 test)"
		exit 1
	fi
else
	echo "Skip 'ipv6logstats' version 3 test (missing STAT_CC support)"
fi

# testscenarios matching
if ./ipv6logstats -v 2>&1 | grep -qw "STAT_CC"; then
	feature_cc=1
fi

if ./ipv6logstats -v 2>&1 | grep -qw "STAT_AS"; then
	feature_as=1
fi

testscenarios_match | while read ip match; do
	echo -n "INFO  : test $ip for match $match: "

	if echo "$match" | grep -q "^AS"; then
		if [ "$feature_as" != "1" ]; then
			echo "SKIPPED (missing support)"
			continue
		fi
	fi

	if echo "$match" | grep -q "^CC"; then
		if [ "$feature_cc" != "1" ]; then
			echo "SKIPPED (missing support)"
			continue
		fi
	fi

	if echo "$ip" | ./ipv6logstats -q | grep -q "$match\W*1"; then
		echo "OK"
	else
		echo "ERROR, unexpected result:"
		echo "$ip" | ./ipv6logstats -q | grep -v "DB-Info"
		exit 1
	fi
done || exit 1

echo "INFO  : test scenario with huge amount of addresses..."
testscenario_hugelist ipv4 | ./ipv6logstats -q >/dev/null
if [ $? -ne 0 ]; then
	echo "ERROR : exit code <> 0"
	exit 1
fi
echo "INFO  : test scenario with huge amount of addresses: OK"

echo "All tests were successfully done!"
