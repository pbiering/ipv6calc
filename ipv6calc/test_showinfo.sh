#!/bin/sh
#
# Project    : ipv6calc
# File       : test_showinfo.sh
# Version    : $Id: test_showinfo.sh,v 1.33 2014/04/22 05:52:49 ds6peter Exp $
# Copyright  : 2002-2011 by Peter Bieringer <pb (at) bieringer.de>
#
# Test patterns for ipv6calc showinfo

source ./test_scenarios.sh


getexamples() {
cat <<END
3FFE:1a05:510:200:0:5EFE:8CAD:8108	# ISATAP
ff02::1:ff00:1234			# Solicited node link-local multicast address
ff01::1:ff00:1234			# Solicited node link-local multicast address
3ffe::1:ff00:1234			# Solicited node 6bone multicast address
::1					# localhost
::1.2.3.4				# compat IPv4
::ffff:1.2.3.4				# mapped IPv4
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
2001:a60:9002:1::186:6
212.18.21.186
END
}

getexamples | while read address separator comment; do
	echo "$comment: $address"
	./ipv6calc -q -i -m $address || exit 1
	echo
done || exit 1

# Test reserved IPv4 addresses
testscenarios_ipv4_reserved | while read address rfc; do
	echo "$rfc: $address"
	case $rfc in
	    RFC*)
		if ! ./ipv6calc -q -i -m $address | grep ^IPV4_REGISTRY | grep "$rfc"; then
			echo "ERROR: unexpected result (should: $rfc)"
			./ipv6calc -q -i -m $address | grep ^IPV4_REGISTRY
			exit 1	
		fi
		;;
	    noRFC)
		if ./ipv6calc -q -i -m $address | grep ^IPV4_REGISTRY | grep "RFC"; then
			echo "ERROR: unexpected result (should not contain RFC token)"
			./ipv6calc -q -i -m $address | grep ^IPV4_REGISTRY
			exit 1	
		fi
		;;
	esac
	echo
done || exit 1

# Test reserved IPv6 addresses
testscenarios_ipv6_reserved | while read address rfc; do
	echo "$rfc: $address"
	if ! ./ipv6calc -q -i -m $address | grep ^IPV6_REGISTRY | grep "$rfc"; then
		echo "ERROR: unexpected result (should: $rfc)"
		./ipv6calc -q -i -m $address | grep ^IPV6_REGISTRY
		exit 1	
	fi
	echo
done || exit 1

# Test showinfo output
ipv6calc_has_db_ieee=0
if ./ipv6calc -v 2>&1 | grep -qw DB_IEEE; then
	ipv6calc_has_db_ieee=1
fi

ipv6calc_has_db_ipv4=0
if ./ipv6calc -v 2>&1 | grep -qw DB_IPV4; then
	ipv6calc_has_db_ipv4=1
fi

ipv6calc_has_db_ipv6=0
if ./ipv6calc -v 2>&1 | grep -qw DB_IPV6; then
	ipv6calc_has_db_ipv6=1
fi

testscenarios_showinfo | while read address output; do
	if echo "$output" | grep -q "^OUI="; then
		if [ $ipv6calc_has_db_ieee -ne 1 ]; then
			echo "Test: $address for $output SKIPPED (no DB_IEEE compiled in)"
			continue
		fi
	fi

	if echo "$output" | grep -q "^IPV4_REGISTRY="; then
		if [ $ipv6calc_has_db_ipv4 -ne 1 ]; then
			echo "Test: $address for $output SKIPPED (no DB_IPV4 compiled in)"
			continue
		fi
	fi

	if echo "$output" | grep -q "^IPV6_REGISTRY="; then
		if [ $ipv6calc_has_db_ipv6 -ne 1 ]; then
			echo "Test: $address for $output SKIPPED (no DB_IPV6 compiled in)"
			continue
		fi
	fi

	echo "Test: $address for $output"
	output_escaped="${output//./\\.}"
	output_escaped="${output_escaped//[/\\[}"
	output_escaped="${output_escaped//]/\\]}"
	if ! ./ipv6calc -q -i -m $address | grep "^$output_escaped$"; then
		echo "ERROR: unexpected result ($output_escaped)"
		./ipv6calc -q -i -m $address
		exit 1	
	fi
	echo
done || exit 1

if [ "$1" = "minimal" ]; then
	echo "GeoIP & IP2Location tests skipped (option 'minimal' used)"
	exit 0
fi

if ./ipv6calc -v 2>&1 | grep -qw GeoIP; then
	echo "Run GeoIP tests"
	getexamples_GeoIP | while read address; do
		echo "Run GeoIP showinfo on: $address"
		if ./ipv6calc -q -i -m -G $address | grep ^GEOIP; then
			true
		else
			if echo "$address" | grep -q ":"; then
				if ./ipv6calc -v 2>&1 | grep -v -q "GeoIPv6"; then
					echo "Expected result (missing GEOIP) because of missing IPv6 support in GeoIP)"
					true
				else
					echo "Unexpected result (missing GEOIP): ./ipv6calc -q -i -m -G $address"
					./ipv6calc -q -i -m -G $address
					exit 1
				fi
			else
				echo "Unexpected result (missing GEOIP): ./ipv6calc -q -i -m -G $address"
				./ipv6calc -q -i -m -G $address
				exit 1
			fi
		fi
	done || exit 1

	testscenarios_showinfo_geoip | while read address output requirement; do
		if echo "$output" | grep -q "^OUI="; then
			if [ $ipv6calc_has_db_ieee -ne 1 ]; then
				echo "Test: $address for $output SKIPPED (no DB_IEEE compiled in)"
				continue
			fi
		fi

		if [ -n "$requirement" ]; then
			if ./ipv6calc -v | grep -q -w "$requirement"; then
				true
			else
				echo "Test: $address for $output SKIPPED (no $requirement compiled in)"
				continue
			fi
		fi
		echo "Test: $address for $output"
		output_escaped="${output//./\\.}"
		output_escaped="${output_escaped//[/\\[}"
		output_escaped="${output_escaped//]/\\]}"
		if ! ./ipv6calc -q -i -m $address | grep "^$output_escaped$"; then
			echo "ERROR: unexpected result ($output_escaped)"
			./ipv6calc -q -i -m $address
			exit 1	
		fi
		echo
	done || exit 1
	echo "GeoIP tests were successful"
else
	echo "GeoIP tests skipped"
fi

if ./ipv6calc -v 2>&1 | grep -qw IP2Location; then
	echo "Run IP2Location tests"
	getexamples_IP2Location | while read address; do
		echo "Run IP2Location showinfo on: $address"
		if ./ipv6calc -q -i -m -L $address | egrep -v '=This (record|parameter) ' | grep ^IP2LOCATION; then
			true
		else
			echo "Unexpected result (missing IP2LOCATION): ./ipv6calc -q -i -m -L $address"
			./ipv6calc -q -i -m -L $address
			exit 1
		fi
	done || exit 1

	testscenarios_showinfo_ip2location | while read address output; do
		if echo "$output" | grep -q "^OUI="; then
			if [ $ipv6calc_has_db_ieee -ne 1 ]; then
				echo "Test: $address for $output SKIPPED (no DB_IEEE compiled in)"
				continue
			fi
		fi
		echo "Test: $address for $output"
		output_escaped="${output//./\\.}"
		output_escaped="${output_escaped//[/\\[}"
		output_escaped="${output_escaped//]/\\]}"
		if ! ./ipv6calc -q -i -m $address | grep "^$output_escaped$"; then
			echo "ERROR: unexpected result ($output_escaped)"
			./ipv6calc -q -i -m $address
			exit 1	
		fi
		echo
	done || exit 1
	echo "IP2Location tests were successful"
else
	echo "IP2Location tests skipped"
fi

if ./ipv6calc -v 2>&1 | grep -qw "ANON_KEEP-TYPE-ASN-CC"; then
	echo "INFO  : run special anon tests"
	testscenarios_showinfo_anonymized_info | while IFS=";" read input options token result; do
		echo -n "INFO  : test: $options $input for $result: "
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
				echo "OK"
			else
				echo " ERROR"
				echo "$output"
				exit 1
			fi
			;;
		    key-word)
			if echo "$output" | grep "^$key=" | grep -w "$word"; then
				echo " OK"
			else
				exit 1
			fi
			;;
		    key-no-word)
			if ! echo "$output" | grep "^$key=" | grep -w "$word"; then
				echo " OK"
			else
				exit 1
			fi
			;;
		esac
	done || exit 1
	echo "special tests were successful"
fi
