#!/bin/sh
#
# Project    : ipv6calc/databases/ipv4-assignment
# File       : ipv6calc-registries-check-run-create.sh
# Version    : $Id$
# Copyright  : 2002-2017 by Peter Bieringer <pb (at) bieringer.de>

flag_update=0

help() {
	cat <<END
Usage: $0 -H|-B -4|-6 -C <command> [-u] [-S <dir>] [-D <dir>]
	-S <dir>	destination directory
	-D <dir>	source directory
	-u		unconditional update
	-d <debuglevel>	debug level
END
}

while getopts "d:uS:D:HB46C:h\?" opt; do
	case $opt in
	    u)
		flag_update="1"
		;;
	    H)
		option="$opt"
		;;
	    B)
		option="$opt"
		;;
	    4)
		proto="$opt"
		file_iana="ipv4-address-space.xml"
		;;
	    6)
		proto="$opt"
		file_iana="ipv6-unicast-address-assignments.xml"
		;;
	    C)
		command=$OPTARG
		;;
	    S)
		src_dir=$OPTARG
		;;
	    D)
		dst_dir=$OPTARG
		;;
	    d)
		debug=$OPTARG
		;;
	    *)
		help
		exit 1
		;;
	esac
done

if [ -z "$command" ]; then
	echo "ERROR : option missing: -C <command>"
	exit 1
fi

case $option in
    H)
	file="dbipv${proto}addr_assignment.h"
	;;
    B)
	file="ipv6calc-external-ipv${proto}-registry.db"
	file2="ipv6calc-external-ipv${proto}-countrycode.db"
	;;
esac

if [ ! -x "$command" ]; then
	echo "ERROR : not found or not executable: $command"
	exit 1
fi

if [ -n "$src_dir" ]; then
	search_dir="$src_dir/"

	if [ ! -d "$src_dir" ]; then
		echo "ERROR : source directory missing: $src_dir"
		exit 1
	fi
else
	search_dir="../registries/"

	if [ ! -d "$search_dir" ]; then
		echo "ERROR : default search directory missing: $search_dir"
		exit 1
	fi
fi


dst_dir=${dst_dir:-.}
if [ ! -d "$dst_dir" ]; then
	echo "ERROR : destination directory missing: $dst_dir"
	exit 1
fi
file="$dst_dir/$file"
file2="$dst_dir/$file2"

echo "INFO  : reference file: $file"
echo "INFO  : search directory: $search_dir"

if [ -f "$file" ]; then
	IANA=`find    $search_dir -type f -name "$file_iana" -newer "$file" | wc -l`
	ARIN=`find    $search_dir -type f -name 'delegated-arin*'    -newer "$file" | wc -l`
	APNIC=`find   $search_dir -type f -name 'delegated-apnic*'   -newer "$file" | wc -l`
	RIPENCC=`find $search_dir -type f -name 'delegated-ripencc*' -newer "$file" | wc -l`
	LACNIC=`find  $search_dir -type f -name 'delegated-lacnic*'  -newer "$file" | wc -l`
	AFRINIC=`find $search_dir -type f -name 'delegated-afrinic*' -newer "$file" | wc -l`
	LISP=`find    $search_dir -type f -name 'site-db*'           -newer "$file" | wc -l`

	echo "INFO  : found newer than $file files: IANA=$IANA ARIN=$ARIN APNIC=$APNIC RIPENCC=$RIPENCC LACNIC=$LACNIC AFRINIC=$AFRINIC LISP=$LISP"

	if [ $IANA -gt 0 -o $ARIN -gt 0 -o $APNIC -gt 0 -o $RIPENCC -gt 0 -o $LACNIC -gt 0 -o $AFRINIC -gt 0 -o $LISP -gt 0 ]; then
		flag_update=1
	elif [ -n "$file2" -a -f "$file2" ]; then
		# 2nd file given
		if [ "$file" -nt "$file2" ]; then
			echo "INFO  : found newer than $file: $file2"
			flag_update=1
		fi
	fi

else
	flag_update=1
fi

[ -n "$src_dir" ] && options="$options -S $src_dir"
[ -n "$dst_dir" ] && options="$options -D $dst_dir"
[ -n "$debug"   ] && options="$options -d $debug"
options="$options -$option -A"

if [ $flag_update -eq 1 ]; then
	echo "INFO  : call now create program with options: $options"
	nice -n 19 $command $options
	rc=$?
else
	echo "NOTICE: nothing to do for proto IPv$proto"
	rc=0
fi

exit $rc
