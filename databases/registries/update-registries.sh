#!/bin/sh
#
# Project    : ipv6calc/databases/registries
# File       : update-registries.sh
# Version    : $Id: update-registries.sh,v 1.9 2011/05/05 19:19:52 peter Exp $
# Copyright  : 2002-2011 by Peter Bieringer <pb (at) bieringer.de>
#               replaces ../ipv4-assignment/update-ipv4-assignment.sh
#               replaces ../ipv6-assignment/update-ipv6-assignment.sh
#
# Information:
#  Shell script to update registry data

#set -x

get_urls() {
# date 2 days ago
year="`date -d '2 days ago' +%Y`"
month="`date -d '2 days ago' +%m`"
day="`date -d '2 days ago' +%d`"

cat <<END | sed s/\%Y/$year/g | sed s/\%m/$month/g | sed s/\%d/$day/g
iana	http://www.iana.org/assignments/ipv4-address-space/			ipv4-address-space.xml			xml	out
iana	http://www.iana.org/assignments/ipv6-unicast-address-assignments/	ipv6-unicast-address-assignments.xml	xml	out
ripencc	ftp://ftp.ripe.net/pub/stats/ripencc/		delegated-ripencc-latest		txt
arin	ftp://ftp.arin.net/pub/stats/arin/		delegated-arin-latest			txt
apnic	http://ftp.apnic.net/stats/apnic/		delegated-apnic-latest			txt
lacnic	ftp://ftp.lacnic.net/pub/stats/lacnic/		delegated-lacnic-latest			txt
afrinic	ftp://ftp.afrinic.net/pub/stats/afrinic/	delegated-afrinic-latest		txt
END
}

echo "Download new version of files"

get_urls | while read subdir url filename format flag; do
	echo "Check: $subdir"
	if [ ! -d "$subdir" ]; then
		mkdir "$subdir" || exit 1
	fi
	pushd $subdir || exit 1
	if [ "$flag" = "out" ]; then
		wget $url$filename -O $filename
	else
		wget $url$filename --timestamping --retr-symlinks
	fi
	retval=$?
	popd
	if [ $retval -ne 0 ]; then
		echo "  Error during download: $subdir/$filename"
		exit 1
	fi

	pushd $subdir || exit 1
	case $format in
            'txt')
		# nothing to do
		;;
            'xml')
		# fix buggy encoding
		perl -pi -e "s/^(.*encoding=')ASCII('.*)$/\1US-ASCII\2/" $filename || exit 1
		;;
	    'bz2')
		# decompress
		bzip2 -f -d -k $filename || exit 1
		;;
	    *)
		echo "ERROR: unsupported format: $format - fix it"
		exit 1
		;;
	esac
	popd

	echo
done
