#!/bin/sh

# $Id: test_showinfo.sh,v 1.4 2006/08/07 19:40:11 peter Exp $


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
END
}

getexamples | while read address separator comment; do
	echo "$comment: $address"
	./ipv6calc -q -i -m -p ../databases/IP2Location/C-IP2Location-1.1.0/data/IP-COUNTRY-SAMPLE.BIN -g ../databases/GeoIP/GeoIP-1.3.17/data/GeoIP.dat $address
	echo
done
