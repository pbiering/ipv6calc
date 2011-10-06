#!/bin/sh
#
# Project    : ipv6calc
# File       : test_showinfo.sh
# Version    : $Id: test_showinfo.sh,v 1.14 2011/10/06 19:32:36 peter Exp $
# Copyright  : 2002-2011 by Peter Bieringer <pb (at) bieringer.de>
#
# Test patterns for ipv6calc showinfo


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
3ffe:831f:ce49:7601:8000:efff:af4a:86BF	# Teredo
2001:a60:f011::1			# Productive IPv6
2001:0010:392e:a450:2cd3:75e1:6098:8104	# Orchid
fe80::8000:5445:5245:444F		# Link-local teredo
fe80::ffff:ffff:fffd			# Link-local teredo
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
done

if ./ipv6calc -v 2>&1 | grep -qw GeoIP; then
	echo "Run GeoIP tests"
	getexamples_GeoIP | while read address; do
		echo "Run GeoIP showinfo on: $address"
		if ./ipv6calc -q -i -m -G $address | grep ^GEOIP; then
			true
		else
			if echo "$address" | grep -q ":"; then
				if ./ipv6calc -v 2>&1 | grep -q "GeoIP (v4only)"; then
					echo "Expected result (missing GEOIP) because of missing IPv6 support in GeoIP)"
					true
				else
					echo "Unexpected result (missing GEOIP):"
					./ipv6calc -q -i -m -G $address
					exit 1
				fi
			else
				echo "Unexpected result (missing GEOIP):"
				./ipv6calc -q -i -m -G $address
				exit 1
			fi
		fi
	done || exit 1
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
			echo "Unexpected result (missing IP2LOCATION):"
			./ipv6calc -q -i -m -L $address
			exit 1
		fi
	done || exit 1
else
	echo "IP2Location tests skipped"

fi
