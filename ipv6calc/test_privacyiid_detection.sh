#!/bin/bash
#
# Project    : ipv6calc
# File       : test_privacyiid_detection.sh
# Version    : $Id: test_privacyiid_detection.sh,v 1.1 2012/03/31 11:24:03 peter Exp $
# Copyright  : 2012-2012 by Peter Bieringer <pb (at) bieringer.de>
#
# Test script for detection of a privacy IID

if [ ! -x ./ipv6calc ]; then
	echo "Binary './ipv6calc' missing or not executable"
	exit 1
fi


max=1000000
i=0

iid="0224:21ff:fe00:0001"
token="0123456789abcdef"

generate() {
	while [ $i -lt $max ]; do
		iidtoken_new="`./ipv6calc -q -F -A genprivacyiid $iid $token`"
		if [ -z "$iidtoken_new" ]; then
			exit 1
		fi
		iid="${iidtoken_new/ *}"
		token="${iidtoken_new/* }"
		#echo "$iid $token"
		ipv6="2001:0db8:0000:0000:$iid"
		echo "$ipv6"
	 	i=$[ $i + 1 ]
	done
}

analyze() {
	for f in $*; do
		echo "INFO : analyze file: $f"
		cat $f | ./ipv6calc --print-iid-var | awk '{
			h = $5;
			if (h > max) { max = h; };
			if (h < min || min == 0) { min = h; };

			print $1 " " h " min=" min " max=" max;
		}' | tail -1
	done

}

case $1 in
    generate)
	generate
	exit 0
	;;
    analyze)
	analyze $2 $3
	exit 0
	;;
esac

