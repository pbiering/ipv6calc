#!/bin/sh
#
# Project    : ipv6calc/databases/ipv4-assignment
# File       : update-ipv4-assignment.sh
# Version    : $Id: update-ipv4-assignment.sh,v 1.3 2004/08/30 19:44:14 peter Exp $
# Copyright  : 2002-2004 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Update shell script, extract file if necessary

#set -x

get_urls() {
# yesterday's date
year="`date -d yesterday +%Y`"
month="`date -d yesterday +%m`"
day="`date -d yesterday +%d`"

cat <<END | sed s/\%Y/$year/g | sed s/\%m/$month/g | sed s/\%d/$day/g
iana	http://www.iana.org/assignments/		ipv4-address-space		txt
ripencc	ftp://ftp.ripe.net/pub/stats/ripencc/%Y/	delegated-ripencc-%Y%m%d.bz2	bz2
arin	ftp://ftp.arin.net/pub/stats/arin/		delegated-arin-%Y%m%d		txt
apnic	http://ftp.apnic.net/stats/apnic/		delegated-apnic-%Y%m%d		txt
lacnic	ftp://lacnic.net/pub/stats/lacnic/		delegated-lacnic-%Y%m%d		txt
END
}

echo "Download new version of files"

get_urls | while read subdir url filename format; do
	echo "Check: $subdir"
	pushd $subdir
	wget $url$filename --timestamping
	retval=$?
	popd
	if [ $retval -ne 0 ]; then
		echo "  Error during download: $subdir/$filename"
		exit 1
	fi

	pushd $subdir
	case $format in
            'txt')
		# nothing to do
		;;
	    'bz2')
		# decompress
		bzip2 -d -k $filename || exit 1
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
