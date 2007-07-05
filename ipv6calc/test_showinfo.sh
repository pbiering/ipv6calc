#!/bin/sh

# $Id: test_showinfo.sh,v 1.8 2007/07/05 20:58:23 peter Exp $


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
END
}

getexamples | while read address separator comment; do
	echo "$comment: $address"
	./ipv6calc -q -i -m --db-ip2location-ipv4 /usr/share/IP2Location/IP-COUNTRY-SAMPLE.BIN --db-ip2location-ipv6 /var/local/share/IP2Location/IPV6-COUNTRY.BIN --db-geoip /usr/share/GeoIP/GeoIP.dat $address || exit 1
	echo
done
