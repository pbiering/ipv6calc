#!/bin/sh
#
# Project    : ipv6calc/databases/ipv4-assignment
# File       : update-ipv4-assignment.sh
# Version    : $Id: update-ipv4-assignment.sh,v 1.1 2002/03/24 16:54:56 peter Exp $
# Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Update shell script

#set -x

get_urls() {
# yesterday's date
year="`date -d yesterday +%Y`"
month="`date -d yesterday +%m`"
day="`date -d yesterday +%d`"

cat <<END | sed s/\%Y/$year/g | sed s/\%m/$month/g | sed s/\%d/$day/g
ripencc	ftp://ftp.ripe.net/ripe/stats/		ripencc.%Y%m%d
arin	ftp://ftp.arin.net/pub/stats/arin/	arin.%Y%m01
apnic	http://ftp.apnic.net/stats/apnic/	apnic-%Y-%m-01
iana	http://www.iana.org/assignments/	ipv4-address-space
END
}

echo "Download new version of files"

get_urls | while read subdir url filename; do
	echo "Check: $subdir"
	pushd $subdir
	wget $url$filename --timestamping
	retval=$?
	popd
	if [ $retval -ne 0 ]; then
		echo "  Error during download: $subdir/$filename"
		exit 1
	fi
	echo
done
exit 0
