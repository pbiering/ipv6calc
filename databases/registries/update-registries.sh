#!/bin/sh
#
# Project    : ipv6calc/databases/registries
# File       : update-registries.sh
# Version    : $Id: update-registries.sh,v 1.2 2005/10/26 09:36:54 peter Exp $
# Copyright  : 2002-2005 by Peter Bieringer <pb (at) bieringer.de>
#               replaces ../ipv4-assignment/update-ipv4-assignment.sh
#               replaces ../ipv6-assignment/update-ipv6-assignment.sh
#
# Information:
#  Update registry data shell script

#set -x

get_urls() {
# date 2 days ago
year="`date -d '2 days ago' +%Y`"
month="`date -d '2 days ago' +%m`"
day="`date -d '2 days ago' +%d`"

cat <<END | sed s/\%Y/$year/g | sed s/\%m/$month/g | sed s/\%d/$day/g
iana	http://www.iana.org/assignments/		ipv4-address-space			txt
iana	http://www.iana.org/assignments/		ipv6-unicast-address-assignments	txt
ripencc	ftp://ftp.ripe.net/pub/stats/ripencc/%Y/	delegated-ripencc-%Y%m%d.bz2		bz2
arin	ftp://ftp.arin.net/pub/stats/arin/		delegated-arin-%Y%m%d			txt
apnic	http://ftp.apnic.net/stats/apnic/		delegated-apnic-%Y%m%d			txt
lacnic	ftp://lacnic.net/pub/stats/lacnic/		delegated-lacnic-%Y%m%d			txt
afrinic	ftp://ftp.afrinic.net/pub/stats/afrinic/	delegated-afrinic-%Y%m%d		txt
END
}

echo "Download new version of files"

get_urls | while read subdir url filename format; do
	echo "Check: $subdir"
	pushd $subdir || exit 1
	wget $url$filename --timestamping
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
exit 0
