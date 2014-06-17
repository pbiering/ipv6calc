#!/bin/bash
#
# Project    : ipv6calc
# File       : autogen-all-variants.sh
# Version    : $Id: autogen-all-variants.sh,v 1.32 2014/06/17 20:21:15 ds6peter Exp $
# Copyright  : 2011-2014 by Peter Bieringer <pb (at) bieringer.de>
#
# Information: run autogen.sh with all supported variants
#
# can run also through various version of GeoIP (-g) and IP2Location (-i) libraries using following directory layout:
#
# builddir/ipv6calc
#          C-IP2Location-4.0.2
#          ip2location-c-6.0.1
#          IP2Location-c-6.0.2
#          GeoIP-1.4.4
#          GeoIP-1.4.5
#          GeoIP-1.4.6
#          GeoIP-1.4.7
#          GeoIP-1.4.8 (broken)
#          GeoIP-1.5.1

status_file="autogen-all-variants.status"

geoip_versions="1.4.4 1.4.5 1.4.6 1.4.7 1.5.1" # 1.4.8 build is broken
ip2location_versions="4.0.2 6.0.1 6.0.2"

geoip_version_latest=$(echo "$geoip_versions" | awk '{ print $NF }')
ip2location_version_latest=$(echo "$ip2location_versions" | awk '{ print $NF }')


## retrieve IP2Location source package name from version
ip2location_name_from_version() {
	local version="$1"

	case $version in
	    4.*)
		local name="C-IP2Location"
		;;
	    6.0.1)
		local name="ip2location-c"
		;;
	    *)
		# default
		local name="IP2Location-c"
		;;
	esac

	echo "$name"
}

# try fallback
if [ ! -e "/usr/include/GeoIP.h" ]; then
	echo "NOTICE: /usr/include/GeoIP.h is missing, check for local availability"

	dir="../GeoIP-$geoip_version_latest"

	if [ -d "$dir" ]; then
		echo "INFO  : found at least directory: $dir"
		geoip_options_extra="--with-geoip-headers=$dir/libGeoIP --with-geoip-lib=$dir/libGeoIP/.libs"
	else
		echo "NOTICE: did not find directory: $dir"
	fi
fi

if [ ! -e "/usr/include/IP2Location.h" ]; then
	echo "NOTICE: /usr/include/IP2Location.h is missing, check for local availability"

	name="$(ip2location_name_from_version $ip2location_version_latest)"
	dir="../$name-$ip2location_version_latest"

	if [ -d "$dir" ]; then
		echo "INFO  : found at least directory: $dir"
		ip2location_options_extra="--with-ip2location-headers=$dir/libIP2Location --with-ip2location-lib=$dir/libIP2Location/.libs"
	else
		echo "NOTICE: did not find directory: $dir"
	fi
fi


## Generate configure variants
autogen_variants() {
	if [ "$ip2location_versions_test" = "1" ]; then
		for version in $ip2location_versions; do
			name=$(ip2location_name_from_version $version)
			echo "IP2LOCATION#--enable-ip2location --with-ip2location-headers=../$name-$version/libIP2Location --with-ip2location-lib=../$name-$version/libIP2Location/.libs"
		done
	fi
	if [ "$geoip_versions_test" = "1" ]; then
		for version in $geoip_versions; do
			echo "GEOIP#--enable-geoip --with-geoip-headers=../GeoIP-$version/libGeoIP --with-geoip-lib=../GeoIP-$version/libGeoIP/.libs"
		done
	fi

	if [ "$geoip_versions_test" = "1" -o "$ip2location_versions_test" = "1" ]; then
		true
		# nothing more to do
	else
		cat <<END | grep -v ^#
# default (no options)
NONE#--enable-bundled-md5 --enable-bundled-getopt
IP2LOCATION#-i
IP2LOCATION#-i --ip2location-dyn
GEOIP#-g
GEOIP#-g --geoip-dyn
GEOIP#-g --geoip-ipv6-compat
GEOIP#-g --geoip-ipv6-compat --geoip-dyn
IP2LOCATION GEOIP#-a
NONE#--disable-db-ieee
NONE#--disable-db-ipv4
NONE#--disable-db-ipv6
NONE#--disable-db-ipv6 --disable-db-ipv4
NONE#--disable-db-ipv6 --disable-db-ipv4 --disable-db-ieee
NONE#--disable-db-ipv6 --disable-db-ieee
NONE#--disable-db-ipv4 --disable-db-ieee
END
	fi
}

help() {
	cat <<END
$0
	-h|-?	this online help
	-f	force run, remove status file
	-W	add option -W (warning) to autogen.sh
	-N	add --no-static-build to autogen.sh
	-I	skip IP2Location builds
	-g	run only through internal defined GeoIP versions
	-i	run only through internal defined IP2Location versions
END
}


while getopts ":NigIfW?h" opt; do
	case $opt in
	    'f')
		force=1
		echo "DEBUG : option found: -f"
		;;
	    'W')
		options_add="$options_add -W"
		;;
	    'N')
		no_static_build=1
		;;
	    'I')
		skip_token="IP2LOCATION"
		;;
	    'g')
		geoip_versions_test="1"
		skip_shared="1"
		skip_basic="1"
		no_static_build=1
		;;
	    'i')
		ip2location_versions_test="1"
		skip_shared="1"
		skip_basic="1"
		no_static_build=1
		;;
	    \?|h)
		help
		exit 1
		;;
	    *)
		echo "Invalid option: -$OPTARG" >&2
		exit 1
		;;
	esac
done

shift $[ $OPTIND - 1 ]

if [ -n "$options_add" ]; then
	echo "INFO  : additional options: $options_add"
fi

if [ -f "$status_file" ]; then
	echo "INFO  : status file found: $status_file"

	if grep -q ":END:" $status_file; then
		if [ "$force" = "1" ]; then
			echo "NOTICE: all runs successful, option -f given, status file removed (re-run)"
			rm $status_file
		else
			echo "NOTICE: all runs successful, nothing more to do (use -f for force a re-run)"
			exit 0
		fi
	else
		if [ "$force" = "1" ]; then
			echo "NOTICE: option -f for forcing a re-run is useless, last run was not finished"
			exit 0
		fi
	fi
fi

if [ ! -f "$status_file" ]; then
	echo "INFO  : status file missing, create: $status_file"
	date "+%s:START:" >$status_file
fi

IONICE="ionice -c 3"

if ! $IONICE true; then
	echo "NOTICE: disable use of ionice, not supported"
	IONICE=""
fi

if [ "$skip_basic" != "1" ]; then
	if grep -q ":FINISHED:basic:$options_add:" $status_file; then
		echo "NOTICE : skip basic run with: $options_add"
	else
		# basic defaults
		nice -n 20 $IONICE ./autogen.sh $options_add
		if [ $? -ne 0 ]; then
			echo "ERROR : 'autogen.sh (basic) $options_add' reports an error"
			exit 1
		fi
		# add entry in log
		date "+%s:FINISHED:basic:$options_add:" >>$status_file
	fi
fi

# variants
for liboption in "normal" "shared"; do
	if [ "$skip_shared" = "1" -a "$liboption" = "shared" ]; then
		continue
	fi

	autogen_variants | while IFS="#" read token buildoptions; do
		if [ -n "$options_add" ]; then
			if [ "$no_static_build" = "1" ]; then
				options="--no-static-build $options_add $buildoptions"
			else
				options="$options_add $buildoptions"
			fi
		else
			if [ "$no_static_build" = "1" ]; then
				options="--no-static-build $buildoptions"
			else
				options="$buildoptions"
			fi
		fi

		case $liboption in
		    shared)
			options="$options -S"
			;;
		esac

		# extend options in fallback case
		if [ -n "$ip2location_options_extra" ]; then
			if echo "$token" | egrep -wq "IP2LOCATION"; then
				options="$options $ip2location_options_extra"
			fi
		fi

		if [ -n "$geoip_options_extra" ]; then
			if echo "$token" | egrep -wq "GEOIP"; then
				options="$options $geoip_options_extra"
			fi
		fi

		# check for already executed option combination
		if egrep -q ":FINISHED:variants:$options:" $status_file; then
			echo "NOTICE : skip variant run with: $options"
		else
			if echo "$token" | egrep -wq "$skip_token"; then
				echo "NOTICE : skip variant because of token: $token"
				date "+%s:FINISHED:variants:$options:SKIPPED" >>$status_file
				continue
			fi

			# run autogen
			echo "INFO  : call: ./autogen.sh $options"

			nice -n 20 $IONICE ./autogen.sh $options
			if [ $? -ne 0 ]; then
				echo "ERROR : 'autogen.sh reports an error with options: $options"
				exit 1
			fi
			# add entry in log
			date "+%s:FINISHED:variants:$options:OK" >>$status_file
		fi

	done || exit 1
done


echo "INFO  : congratulations, all variants built successful!"
date "+%s:END:" >>$status_file
cat $status_file

make autoclean >/dev/null 
if [ $? -ne 0 ]; then
	echo "ERROR : 'make autoclean' failed"
	exit 1
fi
