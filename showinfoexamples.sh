#!/bin/sh

# $Id: showinfoexamples.sh,v 1.2 2002/02/25 21:18:51 peter Exp $


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
END
}

getexamples | while read address separator comment; do
	echo "$comment: $address"
	./ipv6calc -i -m $address
	echo
done
