#!/bin/sh

# 20020120/PB: initial


getexamples() {
cat <<END
3FFE:1a05:510:200:0:5EFE:8CAD:8108	# ISATAP
ff02::1:ff00:1234			# Solicted node link-local multicast address
ff01::1:ff00:1234			# Solicted node link-local multicast address
3ffe::1:ff00:1234			# Solicted node link-local multicast address
END
}

getexamples | while read address separator comment; do
	echo "$comment: $address"
	echo
	./ipv6calc -i $address
	echo
	echo
done
