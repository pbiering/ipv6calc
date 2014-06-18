#!/bin/bash
#
# Project    : ipv6calc
# File       : autogen-support.sh
# Version    : $Id: autogen-support.sh,v 1.2 2014/06/18 06:18:58 ds6peter Exp $
# Copyright  : 2011-2014 by Peter Bieringer <pb (at) bieringer.de>
#
# Information: provide support funtions to autogen.sh/autogen-all-variants.sh
#
# $BASE_DEVEL_GEOIP/  (default if unset: "..")
#          GeoIP-1.4.4
#          GeoIP-1.4.5
#          GeoIP-1.4.6
#          GeoIP-1.4.7
#          GeoIP-1.4.8
#          GeoIP-1.5.1
#
# $BASE_DEVEL_IP2LCATION/   (default if unset: "..")
#          C-IP2Location-4.0.2
#          ip2location-c-6.0.1
#          IP2Location-c-6.0.2

#### Definitions

## List of GeoIP versions (append newest one rightmost!)
geoip_versions="1.4.4 1.4.5 1.4.6 1.4.7 1.4.8 1.5.1"

## List of IP2Location versions (append newest one rightmost!)
ip2location_versions="4.0.2 6.0.1 6.0.2"

#### NO CHANGES BELOW

### Automatic Definitions

### base directory for GeoIP/IP2Location
BASE_DEVEL_GEOIP=${BASE_DEVEL_GEOIP:-..}
BASE_DEVEL_IP2LOCATION=${BASE_DEVEL_IP2LOCATION:-..}


### Functions Definitions

## retrieve GeoIP/IP2Location source package name from version
nameversion_from_name_version() {
	local name="$1"
	local version="$2"

	local nameversion=""

	case $name in
	    GeoIP)
		nameversion="GeoIP-$version"
		;;
	    IP2Location)
		case $version in
		    4.*)
			nameversion="C-IP2Location-$version"
			;;
		    6.0.1)
			nameversion="ip2location-c-$version"
			;;
		    *)
			# default
			nameversion="IP2Location-c-$version"
			;;
		esac
		;;
	    *)
		echo "ERROR : unsupported: $name" >&2
		return 1
		;;
	esac

	echo "$nameversion"
}


## fallback for GeoIP
fallback_GeoIP() {
	if [ -e "/usr/include/GeoIP.h" ]; then
		echo "NOTICE: /usr/include/GeoIP.h is existing, no fallback required"
		return 0
	fi

	echo "NOTICE: /usr/include/GeoIP.h is missing, check for local availability"

	geoip_options_extra=""

	for version in $(echo "$geoip_versions" | awk '{ for (i=NF;i>0;i--) print $i }'); do
		echo "DEBUG : check for GeoIP version: $version"

		dir="$BASE_DEVEL_GEOIP/$(nameversion_from_name_version GeoIP $version)"

		if [ -d "$dir" ]; then
			echo "INFO  : found at least directory: $dir"
			geoip_options_extra="--with-geoip-headers=$dir/libGeoIP --with-geoip-lib=$dir/libGeoIP/.libs"
			break
		else
			echo "NOTICE: did not find directory: $dir (try next)"
		fi
	done

	if [ -z "$geoip_options_extra" ]; then
		echo "ERROR : can't find any local GeoIP source in: $BASE_DEVEL_GEOIP"
		return 1
	else
		echo "INFO  : geoip_options_extra=$geoip_options_extra"
	fi
}

## fallback for IP2Location
fallback_IP2Location() {
	if [ -e "/usr/include/IP2Location.h" ]; then
		echo "NOTICE: /usr/include/IP2Location.h is existing, no fallback required"
		return 0
	fi

	echo "NOTICE: /usr/include/IP2Location.h is missing, check for local availability"

	ip2location_options_extra=""

	for version in $(echo "$ip2location_versions" | awk '{ for (i=NF;i>0;i--) print $i }'); do
		echo "DEBUG : check for IP2Location version: $version"

		dir="${BASE_DEVEL_IP2LOCATION}/$(nameversion_from_name_version IP2Location $version)"

		if [ -d "$dir" ]; then
			echo "INFO  : found at least directory: $dir"
			ip2location_options_extra="--with-ip2location-headers=$dir/libIP2Location --with-ip2location-lib=$dir/libIP2Location/.libs"
			break
		else
			echo "NOTICE: did not find directory: $dir (try next)"
		fi
	done

	if [ -z "$ip2location_options_extra" ]; then
		echo "ERROR : can't find any local IP2Location source in $BASE_DEVEL_IP2LOCATION"
		return 1
	else
		echo "INFO  : ip2location_options_extra=$ip2location_options_extra"
	fi
}


## build GeoIP/IP2Location libraries
build_library() {
	local name="$1"

	local versions=""
	local base_devel=""

	case $name in
	    GeoIP)
		versions="$geoip_versions"
		base_devel="$BASE_DEVEL_GEOIP"
		;;
	    IP2Location)
		versions="$ip2location_versions"
		base_devel="$BASE_DEVEL_IP2LOCATION"
		;;
	    *)
		echo "ERROR : unsupported: $name"
		return 1
		;;
	esac

	result_all=0

	for version in $versions; do
		local nameversion=$(nameversion_from_name_version $name $version)
		echo "INFO  : build library: $name-$version ($nameversion)"

		pushd $base_devel/$nameversion
		if [ $? -ne 0 ]; then
			echo "ERROR : can't change to directory: $base_devel/$nameversion (skip)"
			continue
		fi

		case $name in
		    GeoIP)
			./configure && make clean && make
			result=$?
			;;
		    IP2Location)
			autoreconf -i && ./configure && make clean && make
			result=$?
			;;
		esac

		popd

		if [ $result -ne 0 ]; then
			echo "ERROR : trouble during build of $name-$version ($nameversion)"
			result_all=1
			break
		else
			echo "INFO  : successful build of $name-$version ($nameversion)"
			build_library_status="$build_library_status $nameversion"
		fi
	done

	return $result_all
}


## help
help() {
	cat <<END
$0 [-F|B]
	-F  : fill GeoIP/IP2Location fallback options in case system has no default
	-B  : build GeoIP/IP2Location libraries for fallback and all-variants

	BASE_DEVEL_GEOIP=$BASE_DEVEL_GEOIP
	BASE_DEVEL_IP2LOCATION=$BASE_DEVEL_IP2LOCATION
END
}


#### Main
while getopts ":FB?h" opt; do
	case $opt in
	    'F')
		fallback_GeoIP
		fallback_IP2Location
		exit 0
		;;
	    'B')
		build_library GeoIP || exit 1
		build_library IP2Location || exit 1
		echo "INFO  : following libaries were built successful: $build_library_status"
		exit 0
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

if [ -z "$1" ]; then
	echo "missing option"
	help
	exit 1
fi
