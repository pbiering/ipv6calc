#!/bin/sh
#
# Project    : ipv6calc
# File       : autogen-all-variants.sh
# Version    : $Id: autogen-all-variants.sh,v 1.16 2014/02/10 07:34:41 ds6peter Exp $
# Copyright  : 2011-2014 by Peter Bieringer <pb (at) bieringer.de>
#
# Information: run autogen.sh with all supported variants

status_file="autogen-all-variants.status"

autgen_variants() {
	cat <<END | grep -v ^#
# default (no options)
--enable-bundled-md5 --enable-bundled-getopt
-i
-i --ip2location-dyn
-g
-g --geoip-dyn
-g --geoip-ipv6-compat
-g --geoip-ipv6-compat --geoip-dyn
-a
--disable-db-ieee
--disable-db-ipv4
--disable-db-ipv6
--disable-db-ipv6 --disable-db-ipv4
--disable-db-ipv6 --disable-db-ipv4 --disable-db-ieee
--disable-db-ipv6 --disable-db-ieee
--disable-db-ipv4 --disable-db-ieee
END
}

options_add="$*"

if [ -n "$options_add" ]; then
	echo "INFO  : additional options: $options_add"
fi

if [ ! -f "$status_file" ]; then
	date "+%s:START:" >$status_file
else
	echo "INFO  : status file found: $status_file"

	if grep -q ":END:" $status_file; then
		echo "NOTICE: all runs successful, nothing more to do"
		exit 0
	fi
fi

if grep -q ":FINISHED:basic:$options_add:" $status_file; then
	echo "NOTICE : skip basic run with: $options_add"
else
	# basic defaults
	nice -n 20 ionice -c idle ./autogen.sh $options_add
	if [ $? -ne 0 ]; then
		echo "ERROR : 'autogen.sh (basic) $options_add' reports an error"
		exit 1
	fi
	# add entry in log
	date "+%s:FINISHED:basic:$options_add:" >>$status_file
fi

# variants
for liboption in "normal" "shared"; do
	autgen_variants | while read buildoptions; do
		if [ -n "$options_add" ]; then
			options="$buildoptions $options_add"
		else
			options="$buildoptions"
		fi
		case $liboption in
		    shared)
			options="$options -S"
			;;
		esac

		if grep -q ":FINISHED:variants:$options:" $status_file; then
			echo "NOTICE : skip variant run with: $options"
		else
			nice -n 20 ionice -c idle ./autogen.sh $options
			if [ $? -ne 0 ]; then
				echo "ERROR : 'autogen.sh reports an error with options: $options"
				exit 1
			fi
			# add entry in log
			date "+%s:FINISHED:variants:$options:" >>$status_file
		fi

	done || exit 1
done


echo "INFO  : congratulations, all variants built successful!"
date "+%s:END:" >>$status_file
cat $status_file

make distclean >/dev/null 
if [ $? -ne 0 ]
	echo "ERROR : 'make distclean' failed"
	exit 1
fi
