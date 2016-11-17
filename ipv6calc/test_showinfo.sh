#!/bin/sh
#
# Project    : ipv6calc
# File       : test_showinfo.sh
# Version    : $Id$
# Copyright  : 2002-2015 by Peter Bieringer <pb (at) bieringer.de>
#
# Test patterns for ipv6calc showinfo

verbose=0
grepopt="-q"
while getopts "Vh\?" opt; do
	case $opt in
	    V)
		verbose=1
		grepopt=""
		;;
	    *)
		echo "$0 [-V]"
		echo "    -V   verbose"
		exit 1
		;;
	esac
done


source ./test_scenarios.sh


getexamples() {
cat <<END
3FFE:1a05:510:200:0:5EFE:8CAD:8108	# ISATAP
ff02::1:ff00:1234			# Solicited node link-local multicast address
ff01::1:ff00:1234			# Solicited node link-local multicast address
3ffe::1:ff00:1234			# Solicited node 6bone multicast address
::1					# localhost
::2.2.3.4				# compat IPv4
::ffff:2.2.3.4				# mapped IPv4
2002:0102:0204::1			# 6to4
fe80::210:a4ff:fe01:2345		# link-local autoconfig EUI-48
fe80::210:a489:ab01:2345		# link-local autoconfig EUI-64
3ffe:ffff::210:a4ff:fe01:2345		# global autoconfig EUI-48
3ffe:ffff::210:a489:ab01:2345		# global autoconfig EUI-64
3ffe:ffff::000:a4ff:fe01:2345		# global manual EUI-48
3ffe:ffff::000:a489:ab01:2345		# global manual EUI-64
fe80::fefc:acff:fe10:fe01		# link-local autoconf on ISDN interface
2001:7b0:ffff::1			# Productive IPv6
3ffe:831f:ce49:7601:8000:efff:af4a:86bf	# Teredo
2001:a60:f011::1			# Productive IPv6
2001:0010:392e:a450:2cd3:75e1:6098:8104	# Orchid
fe80::8000:5445:5245:444F		# Link-local teredo
fe80::ffff:ffff:fffd			# Link-local teredo
fe80--1.IPV6-LITERAL.NET                                                       # ipv6literal
2001-db8-0-0-0-0-0-1.ipv6-literal.net                                          # ipv6literal
2001-0db8-0000-0000-0000-0000-0000-0001.ipv6-literal.net                       # ipv6literal
2001-db8--1.ipv6-literal.net                                                   # ipv6literal
fe80--218-8bff-fe17-a226s4.ipv6-literal.net                                    # ipv6literal
END
}

getexamples_GeoIP() {
	cat <<END
2001:a60:9002:1::186:6
212.18.21.186
END
}

getexamples_IP2Location() {
	cat <<END
212.18.21.186
END
}

getexamples_IP2Location6() {
	cat <<END
2a04::1
END
}

getexamples_DBIPv4() {
	cat <<END
212.18.21.186
END
}

getexamples_DBIPv6() {
	cat <<END
2001:a60:9002:1::186:6
END
}

test="test showinfo"
echo "INFO  : $test"
getexamples | while read address separator comment; do
	[ "$verbose" = "1" ] && echo "$comment: $address"
	if [ "$verbose" = "1" ]; then
		./ipv6calc -q -i -m $address
		retval=$?
	else
		./ipv6calc -q -i -m $address >/dev/null
		retval=$?
	fi
	if [ $retval -ne 0 ]; then
		echo "ERROR : $comment: $address"
		./ipv6calc -q -i -m $address
		exit 1
	fi
	[ "$verbose" = "1" ] && echo || true
	[ "$verbose" = "1" ] || echo -n "."
done || exit 1
[ "$verbose" = "1" ] || echo
echo "INFO  : $test successful"

# Test reserved IPv4 addresses
test="test IPv4 reserved"
echo "INFO  : $test"
testscenarios_ipv4_reserved | while read address rfc rest; do
	[ "$verbose" = "1" ] && echo "INFO  : test $rfc: $address"
	case $rfc in
	    RFC*)
		if ! ./ipv6calc -q -i -m $address | grep ^IPV4_REGISTRY | grep $grepopt "$rfc"; then
			echo "ERROR: unexpected result for $address (should: $rfc)"
			./ipv6calc -q -i -m $address | grep ^IPV4_REGISTRY
			exit 1	
		fi
		;;
	    noRFC)
		if ./ipv6calc -q -i -m $address | grep ^IPV4_REGISTRY | grep $grepopt "RFC"; then
			echo "ERROR: unexpected result for $address (should not contain RFC token)"
			./ipv6calc -q -i -m $address | grep ^IPV4_REGISTRY
			exit 1	
		fi
		;;
	esac
	[ "$verbose" = "1" ] && echo || true
	[ "$verbose" = "1" ] || echo -n "."
done || exit 1
[ "$verbose" = "1" ] || echo
echo "INFO  : $test successful"

# Test reserved IPv6 addresses
test="test IPv6 reserved"
echo "INFO  : $test"
testscenarios_ipv6_reserved | while read address rfc rest; do
	[ "$verbose" = "1" ] && echo "INFO  : test $rfc: $address"
	case $rfc in
	    RFC*)
		if ! ./ipv6calc -q -i -m $address | grep ^IPV6_REGISTRY | grep $grepopt "$rfc"; then
			echo "ERROR: unexpected result (should: $rfc)"
			./ipv6calc -q -i -m $address | grep ^IPV6_REGISTRY
			exit 1	
		fi
		;;
	    noRFC)
		if ./ipv6calc -q -i -m $address | grep ^IPV6_REGISTRY | grep $grepopt "RFC"; then
			echo "ERROR: unexpected result (should not contain RFC token)"
			./ipv6calc -q -i -m $address | grep ^IPV6_REGISTRY
			exit 1	
		fi
		;;
	esac
	[ "$verbose" = "1" ] && echo || true
	[ "$verbose" = "1" ] || echo -n "."
done || exit 1
[ "$verbose" = "1" ] || echo
echo "INFO  : $test successful"

# Test showinfo output
ipv6calc_has_db_ieee=0
if ./ipv6calc -v 2>&1 | grep -qw DB_IEEE; then
	ipv6calc_has_db_ieee=1
fi

ipv6calc_has_db_ipv4=0
if ./ipv6calc -v 2>&1 | grep -qw DB_IPV4_REG; then
	ipv6calc_has_db_ipv4=1
fi

ipv6calc_has_db_ipv6=0
if ./ipv6calc -v 2>&1 | grep -qw DB_IPV6_REG; then
	ipv6calc_has_db_ipv6=1
fi

test="test showinfo"
echo "INFO  : $test"
testscenarios_showinfo | while read address output_options; do
	# separate options from output
	output=${output_options/\|*/}
	options=${output_options/*\|/}
	[ "$options" = "$output" ] && options=""

	if echo "$output" | grep -q "^OUI="; then
		if [ $ipv6calc_has_db_ieee -ne 1 ]; then
			echo "NOTICE: test $address for $output SKIPPED (no DB_IEEE compiled in)"
			continue
		fi
	fi

	if echo "$output" | grep -q "^IPV4_REGISTRY="; then
		if [ $ipv6calc_has_db_ipv4 -ne 1 ]; then
			echo "NOTICE: test: $address for $output SKIPPED (no DB_IPV4_REG compiled in)"
			continue
		fi
	fi

	if echo "$output" | grep -q "^IPV6_REGISTRY="; then
		if [ $ipv6calc_has_db_ipv6 -ne 1 ]; then
			echo "NOTICE: test: $address for $output SKIPPED (no DB_IPV6_REG compiled in)"
			continue
		fi
	fi

	[ "$verbose" = "1" ] && echo "INFO: test: $address for $output ($options)"
	output_escaped="${output//./\\.}"
	output_escaped="${output_escaped//[/\\[}"
	output_escaped="${output_escaped//]/\\]}"
	if ! ./ipv6calc $options -q -i -m $address | grep $grepopt "^$output_escaped$"; then
		echo "ERROR: unexpected result for $address ($output_escaped)"
		./ipv6calc $options -q -i -m $address
		exit 1	
	fi
	[ "$verbose" = "1" ] && echo || true
	[ "$verbose" = "1" ] || echo -n "."
done || exit 1
[ "$verbose" = "1" ] || echo
echo "INFO  : $test successful"

if [ "$1" = "minimal" ]; then
	echo "NOTICE: GeoIP/IP2Location/DBIP tests skipped (option 'minimal' used)"
	exit 0
fi


test="run GeoIP tests"
if ./ipv6calc -q -v 2>&1 | grep -qw GeoIP; then
	echo "INFO  : $test"
	getexamples_GeoIP | while read address; do
		[ "$verbose" = "1" ] && echo "INFO  : run GeoIP showinfo on: $address"
		if ./ipv6calc -q -i -m $address | grep $grepopt ^GEOIP; then
			true
		else
			if echo "$address" | grep -q ":"; then
				if ./ipv6calc -q -v 2>&1 | grep -v -q "GeoIPv6"; then
					echo "Expected result (missing GEOIP) because of missing IPv6 support in GeoIP)"
					true
				else
					echo "Unexpected result (missing GEOIP): ./ipv6calc -q -i -m $address"
					./ipv6calc -q -i -m $address
					exit 1
				fi
			else
				echo "Unexpected result (missing GEOIP): ./ipv6calc -q -i -m $address"
				./ipv6calc -q -i -m $address
				exit 1
			fi
		fi
		[ "$verbose" = "1" ] || echo -n "."
	done || exit 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"

	test="run GeoIP showinfo tests"
	echo "INFO  : $test"
	testscenarios_showinfo_geoip | while read address output requirement; do
		if echo "$output" | grep -q "^OUI="; then
			if [ $ipv6calc_has_db_ieee -ne 1 ]; then
				[ "$verbose" = "1" ] && echo "Test: $address for $output SKIPPED (no DB_IEEE compiled in)"
				continue
			fi
		fi

		if [ -n "$requirement" ]; then
			if ./ipv6calc -v 2>&1 | grep -q -w "$requirement"; then
				true
			else
				[ "$verbose" = "1" ] && echo "Test: $address for $output SKIPPED (no $requirement compiled in)"
				continue
			fi
		fi
		[ "$verbose" = "1" ] && echo "Test: $address for $output"
		output_escaped="${output//./\\.}"
		output_escaped="${output_escaped//[/\\[}"
		output_escaped="${output_escaped//]/\\]}"
		if ! ./ipv6calc -q -i -m $address | grep $grepopt "^$output_escaped$"; then
			echo "ERROR: unexpected result for $address ($output_escaped)"
			./ipv6calc -q -i -m $address
			exit 1	
		fi
		[ "$verbose" = "1" ] || echo -n "."
	done || exit 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"
else
	echo "NOTICE: $test SKIPPED"
fi


test="run IP2Location tests"
if ./ipv6calc -q -v 2>&1 | grep -qw IP2Location; then
	echo "INFO  : $test"
	getexamples_IP2Location | while read address; do
		[ "$verbose" = "1" ] && echo "Run IP2Location IPv4 showinfo on: $address"
		if ./ipv6calc -q -i -m $address | egrep -v '=This (record|parameter) ' | grep $grepopt ^IP2LOCATION; then
			true
		else
			echo "Unexpected result (missing IP2LOCATION): ./ipv6calc -q -i -m $address"
			./ipv6calc -q -i -m $address
			exit 1
		fi
		[ "$verbose" = "1" ] && echo
		[ "$verbose" = "1" ] || echo -n "."
	done || exit 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"

	test="run IP2Location showinfo tests"
	echo "INFO  : $test"
	testscenarios_showinfo_ip2location | while read address output; do
		if echo "$output" | grep -q "^OUI="; then
			if [ $ipv6calc_has_db_ieee -ne 1 ]; then
				[ "$verbose" = "1" ] && echo "Test: $address for $output SKIPPED (no DB_IEEE compiled in)"
				continue
			fi
		fi
		[ "$verbose" = "1" ] && echo "Test: $address for $output"
		output_escaped="${output//./\\.}"
		output_escaped="${output_escaped//[/\\[}"
		output_escaped="${output_escaped//]/\\]}"
		if ! ./ipv6calc -q -i -m $address | grep $grepopt "^$output_escaped$"; then
			echo "ERROR: unexpected result for address $address ($output_escaped)"
			./ipv6calc -q -i -m $address
			exit 1	
		fi
		[ "$verbose" = "1" ] && echo
		[ "$verbose" = "1" ] || echo -n "."
	done || exit 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"

	if ./ipv6calc -q -v 2>&1 | grep -qw IP2Location6; then
		test="run IP2Location IPv6 tests"
		echo "INFO  : $test"
		getexamples_IP2Location6 | while read address; do
			[ "$verbose" = "1" ] && echo "Run IP2Location IPv6 showinfo on: $address"
			if ./ipv6calc -q -i -m $address | egrep -v '=This (record|parameter) ' | grep $grepopt ^IP2LOCATION; then
				true
			else
				echo "Unexpected result (missing IP2LOCATION): ./ipv6calc -q -i -m $address"
				./ipv6calc -q -i -m $address
				exit 1
			fi
			[ "$verbose" = "1" ] && echo
			[ "$verbose" = "1" ] || echo -n "."
		done || exit 1
		[ "$verbose" = "1" ] || echo
		echo "INFO  : $test successful"

		test="run IP2Location IPv6 showinfo tests"
		echo "INFO  : $test"
		testscenarios_showinfo_ip2location6 | while read address output; do
			if echo "$output" | grep -q "^OUI="; then
				if [ $ipv6calc_has_db_ieee -ne 1 ]; then
					echo "Test: $address for $output SKIPPED (no DB_IEEE compiled in)"
					continue
				fi
			fi
			[ "$verbose" = "1" ] && echo "Test: $address for $output"
			output_escaped="${output//./\\.}"
			output_escaped="${output_escaped//[/\\[}"
			output_escaped="${output_escaped//]/\\]}"
			if ! ./ipv6calc -q -i -m $address | grep $grepopt "^$output_escaped$"; then
				echo "ERROR: unexpected result for $address ($output_escaped)"
				./ipv6calc -q -i -m $address
				exit 1	
			fi
			[ "$verbose" = "1" ] && echo
			[ "$verbose" = "1" ] || echo -n "."
		done || exit 1
		[ "$verbose" = "1" ] || echo
		echo "INFO  : $test successful"
	fi
else
	echo "NOTICE: $test SKIPPED"
fi


test="run db-ip.com IPv4 tests"
if ./ipv6calc -q -v 2>&1 | grep -qw DBIPv4; then
	echo "INFO  : $test"
	getexamples_DBIPv4 | while read address; do
		[ "$verbose" = "1" ] && echo "Run IP2Location showinfo on: $address"
		if ./ipv6calc -q -i -m $address | egrep -v '=This (record|parameter) ' | grep $grepopt ^DBIP; then
			true
		else
			echo "Unexpected result (missing DBIP): ./ipv6calc -q -i -m $address"
			./ipv6calc -q -i -m $address
			exit 1
		fi
		[ "$verbose" = "1" ] && echo
		[ "$verbose" = "1" ] || echo -n "."
	done || exit 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"

	test="run db-ip.com IPv4 showinfo tests"
	echo "INFO  : $test"
	testscenarios_showinfo_DBIPv4 | while read address output; do
		if echo "$output" | grep -q "^OUI="; then
			if [ $ipv6calc_has_db_ieee -ne 1 ]; then
				[ "$verbose" = "1" ] && echo "Test: $address for $output SKIPPED (no DB_IEEE compiled in)"
				continue
			fi
		fi
		[ "$verbose" = "1" ] && echo "Test: $address for $output"
		output_escaped="${output//./\\.}"
		output_escaped="${output_escaped//[/\\[}"
		output_escaped="${output_escaped//]/\\]}"
		if ! ./ipv6calc -q -i -m $address | grep $grepopt "^$output_escaped$"; then
			echo "ERROR: unexpected result ($output_escaped)"
			./ipv6calc -q -i -m $address
			exit 1	
		fi
		[ "$verbose" = "1" ] && echo
		[ "$verbose" = "1" ] || echo -n "."
	done || exit 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"
else
	echo "NOTICE: $test SKIPPED"
fi


test="run db-ip.com IPv6 tests"
if ./ipv6calc -q -v 2>&1 | grep -qw DBIPv6; then
	echo "INFO  : $test"
	getexamples_DBIPv6 | while read address; do
		[ "$verbose" = "1" ] && echo "Run IP2Location showinfo on: $address"
		if ./ipv6calc -q -i -m $address | egrep -v '=This (record|parameter) ' | grep $grepopt ^DBIP; then
			true
		else
			echo "Unexpected result (missing DBIP): ./ipv6calc -q -i -m $address"
			./ipv6calc -q -i -m $address
			exit 1
		fi
		[ "$verbose" = "1" ] && echo
		[ "$verbose" = "1" ] || echo -n "."
	done || exit 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"

	test="run db-ip.com IPv6 showinfo tests"
	echo "INFO  : $test"
	testscenarios_showinfo_DBIPv6 | while read address output; do
		if echo "$output" | grep -q "^OUI="; then
			if [ $ipv6calc_has_db_ieee -ne 1 ]; then
				echo "Test: $address for $output SKIPPED (no DB_IEEE compiled in)"
				continue
			fi
		fi
		[ "$verbose" = "1" ] && echo "Test: $address for $output"
		output_escaped="${output//./\\.}"
		output_escaped="${output_escaped//[/\\[}"
		output_escaped="${output_escaped//]/\\]}"
		if ! ./ipv6calc -q -i -m $address | grep $grepopt "^$output_escaped$"; then
			echo "ERROR: unexpected result ($output_escaped)"
			./ipv6calc -q -i -m $address
			exit 1	
		fi
		[ "$verbose" = "1" ] && echo
		[ "$verbose" = "1" ] || echo -n "."
	done || exit 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"
else
	echo "NOTICE: $test SKIPPED"
fi


test="run special anon tests"
if ./ipv6calc -v 2>&1 | grep -qw "ANON_KEEP-TYPE-ASN-CC"; then
	echo "INFO  : $test"
	testscenarios_showinfo_anonymized_info | while IFS=";" read input options token result; do
		[ "$verbose" = "1" ] && echo -n "INFO  : test: $options $input for $result: "
		output="`./ipv6calc -q $options -m -i $input`"

		case $token in
		    key-word|key-no-word)
			key="${result/=*/}"
			word="${result/*=/}"
			;;
		esac

		case $token in
		    match)
			if echo "$output" | grep -q "^$result$"; then
				[ "$verbose" = "1" ] && echo "OK"
			else
				echo " ERROR"
				echo "$output"
				exit 1
			fi
			;;
		    key-word)
			if echo "$output" | grep "^$key=" | grep $grepopt -w "$word"; then
				[ "$verbose" = "1" ] && echo " OK"
			else
				exit 1
			fi
			;;
		    key-no-word)
			if ! echo "$output" | grep "^$key=" | grep $grepopt -w "$word"; then
				[ "$verbose" = "1" ] && echo " OK"
			else
				exit 1
			fi
			;;
		esac
		[ "$verbose" = "1" ] || echo -n "."
	done || exit 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"
else
	echo "NOTICE: $test SKIPPED"
fi
