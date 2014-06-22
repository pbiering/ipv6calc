#!/bin/bash
#
# Project    : ipv6calc
# File       : autogen-support.sh
# Version    : $Id: autogen-support.sh,v 1.17 2014/06/22 09:49:25 ds6peter Exp $
# Copyright  : 2014-2014 by Peter Bieringer <pb (at) bieringer.de>
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
geoip_versions_download="$geoip_versions"
geoip_url_base="http://geolite.maxmind.com/download/geoip/api/c/"

geoip_cross_version_test_blacklist() {
	local version_have=$(echo $1 | awk -F. '{ print $3 + $2 * 100 + $1 * 10000}')
	local version_test=$(echo $2 | awk -F. '{ print $3 + $2 * 100 + $1 * 10000}')

	if [ $version_have -eq $version_test ]; then
		# same version
		return 1
	fi

	if [ $version_have -ge 10407 -a $version_test -lt 10407 ]; then
		# missing GeoIP_cleanup
		return 1
	fi

	return 0
}


## List of IP2Location versions (append newest one rightmost!)
ip2location_versions="4.0.2 6.0.1 6.0.2"
ip2location_versions_download="6.0.1 6.0.2" # older versions are no longer available for download?
ip2location_url_base="https://www.ip2location.com/downloads/"

ip2location_cross_version_test_blacklist() {
	return 0
}

#### NO CHANGES BELOW

### Automatic Definitions

### base directory for GeoIP/IP2Location
BASE_SOURCES=${BASE_SOURCES:-~/Downloads}

BASE_DEVEL_GEOIP=${BASE_DEVEL_GEOIP:-~/tmp}
BASE_DEVEL_IP2LOCATION=${BASE_DEVEL_IP2LOCATION:-~/tmp}


### Functions Definitions

## generate GeoIP/IP2Location source package name from version
nameversion_from_name_version() {
	local name="$1"
	local version="$2"
	local mode="$3" # optional

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
		    *)
			# default
			if [ "$mode" = "download" ]; then
				nameversion="ip2location-c-$version"
			else
				case $version in
				    6.0.1)
					nameversion="ip2location-c-$version"
					;;
				    *)
					# default
					nameversion="IP2Location-c-$version"
					;;
				esac
			fi
		esac
		;;
	    *)
		echo "ERROR : unsupported: $name" >&2
		return 1
		;;
	esac

	echo "$nameversion"
}

## retrieve GeoIP/IP2Location options from version
options_from_name_version() {
	local name="$1"
	local version="$2"
	local output="$3"

	local nameversion=$(nameversion_from_name_version $name $version)

	case $name in
	    GeoIP)
		local dir="$BASE_DEVEL_GEOIP/$nameversion"
		libdir="$dir/libGeoIP/.libs"
		lib="$libdir/GeoIP.so"
		result="--with-geoip-headers=$dir/libGeoIP --with-geoip-lib=$libdir"
		;;
	    IP2Location)
		local dir="$BASE_DEVEL_IP2LOCATION/$nameversion"
		libdir="$dir/libIP2Location/.libs"
		lib="$libdir/IP2Location.so"
		result="--with-ip2location-headers=$dir/libIP2Location --with-ip2location-lib=$libdir"
		;;
	    *)
		echo "ERROR : unsupported: $name" >&2
		return 1
		;;
	esac

	case $output in
	    'only-lib')
		echo "$lib"
		;;
	    *)
		echo "$result"
		;;
	esac
}

## fallback for GeoIP/IP2Location
fallback_options_from_name() {
	local name="$1"

	local file_header=""
	local versions=""

	case $name in
	    GeoIP)
		file_header="/usr/include/GeoIP.h"
		versions="$geoip_versions"
		dir_base="$BASE_DEVEL_GEOIP"
		;;
	    IP2Location)
		file_header="/usr/include/IP2Location.h"
		versions="$ip2location_versions"
		dir_base="$BASE_DEVEL_IP2LOCATION"
		;;
	    *)
		echo "ERROR : unsupported: $name" >&2
		return 1
		;;
	esac

	if [ -e "$file_header" ]; then
		echo "NOTICE: file is existing, no fallback required for $name: $file_header" >&2
		return 0
	fi

	echo "NOTICE: file is missing, check for local availability for $name: $file_header" >&2

	for version in $(echo "$versions" | awk '{ for (i=NF;i>0;i--) print $i }'); do
		echo "DEBUG : check for version for $name: $version" >&2

		dir="$dir_base/$(nameversion_from_name_version $name $version)"

		if [ -d "$dir" ]; then
			echo "INFO  : found at least directory for name $name: $dir" >&2
			result="$(options_from_name_version $name $version)"
			break
		else
			echo "NOTICE: did not find directory for name $name: $dir (try next)"
		fi
	done

	if [ -z "$result" ]; then
		echo "ERROR : can't find any local source for $name in: $dir_base"
		return 1
	fi

	echo "$result"
}


## build GeoIP/IP2Location libraries
build_library() {
	local name="$1"
	local version_selected="$2"

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
		if [ -n "$version_selected" -a "$version" != "$version_selected" ]; then
			echo "NOTICE: skip not selected version: $version"
			continue
		fi

		local nameversion=$(nameversion_from_name_version $name $version)

		if [ ! -d "$base_devel/$nameversion" ]; then
			echo "ERROR : devel directory missing: $base_devel/$nameversion (forgot to extract?)"
			return 1
		fi

		pushd $base_devel/$nameversion >/dev/null
		if [ $? -ne 0 ]; then
			echo "ERROR : can't change to directory: $base_devel/$nameversion (skip)"
			continue
		fi

		if [ "$dry_run" = "1" ]; then
			echo "INFO  : would build library (dry-run): $name-$version ($nameversion)"
			continue
		else
			echo "INFO  : build library: $name-$version ($nameversion)"
		fi

		case $name in
		    GeoIP)
			./configure && make clean && make
			result=$?
			;;
		    IP2Location)
			autoreconf -fi && ./configure && make clean && make
			result=$?
			;;
		esac

		popd >/dev/null

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


## clean GeoIP/IP2Location libraries
clean_versions() {
	local name="$1"
	local version_selected="$2"

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
		if [ -n "$version_selected" -a "$version" != "$version_selected" ]; then
			echo "NOTICE: skip not selected version: $version"
			continue
		fi

		local nameversion=$(nameversion_from_name_version $name $version)

		if [ ! -d "$base_devel/$nameversion" ]; then
			echo "ERROR : devel directory missing: $base_devel/$nameversion (forgot to extract?)"
			continue
		fi

		if [ "$dry_run" = "1" ]; then
			echo "INFO  : would remove: $base_devel/$nameversion"
			continue
		else
			echo "INFO  : remove: $base_devel/$nameversion"
		fi

		rm -rf $base_devel/$nameversion
		result=$?

		if [ $result -ne 0 ]; then
			echo "ERROR : trouble during remove of of $name-$version ($nameversion)"
			result_all=1
			break
		else
			echo "INFO  : successful remove of $name-$version ($nameversion)"
			clean_library_status="$clean_library_status $nameversion"
		fi
	done

	return $result_all
}
## extract GeoIP/IP2Location source packages
extract_versions() {
	local name="$1"
	local version_selected="$2"

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
		if [ -n "$version_selected" -a "$version" != "$version_selected" ]; then
			echo "NOTICE: skip not selected version: $version"
			continue
		fi

		local nameversion=$(nameversion_from_name_version $name $version download)
		local file="$BASE_SOURCES/$nameversion.tar.gz"

		if [ ! -f "$file" ]; then
			echo "NOTICE: file not existing, can't extract: $file"
			continue
		fi

		if [ "$dry_run" = "1" ]; then
			echo "INFO  : would extract source package (dry-run): $name-$version ($nameversion) from $file"
			continue
		else
			echo "INFO  : extract source package: $name-$version ($nameversion): $file"
		fi

		if [ ! -d "$base_devel" ]; then
			echo "ERROR : base devel directory missing: $base_devel"
			return 1
		fi

		tar xzf "$file" -C $base_devel
		result=$?

		if [ $result -ne 0 ]; then
			echo "ERROR : trouble during extract of $name-$version ($nameversion) from $file"
			result_all=1
			break
		else
			echo "INFO  : successful extract of $name-$version ($nameversion) from $file to $base_devel"
			extract_library_status="$extract_library_status $nameversion"
		fi
	done

	return $result_all
}
## retrieve GeoIP/IP2Location source packages
download_versions() {
	local name="$1"
	local version_selected="$2"

	case $name in
	    GeoIP)
		versions="$geoip_versions_download"
		base_devel="$BASE_DEVEL_GEOIP"
		base_url="$geoip_url_base"
		;;
	    IP2Location)
		versions="$ip2location_versions_download"
		base_devel="$BASE_DEVEL_IP2LOCATION"
		base_url="$ip2location_url_base"
		;;
	    *)
		echo "ERROR : unsupported: $name"
		return 1
		;;
	esac

	result_all=0

	for version in $versions; do
		if [ -n "$version_selected" -a "$version" != "$version_selected" ]; then
			echo "NOTICE: skip not selected version: $version"
			continue
		fi

		local nameversion=$(nameversion_from_name_version $name $version download)
		local url="$base_url$nameversion.tar.gz"

		if [ "$dry_run" = "1" ]; then
			echo "INFO  : would download source package (dry-run): $name-$version ($nameversion) from $url"
			continue
		else
			echo "INFO  : download source package: $name-$version ($nameversion)"
		fi

		if [ ! -d "$BASE_SOURCES" ]; then
			echo "ERROR : base source directory missing: $BASE_SOURCES (BASE_SOURCES)"
			return 1
		fi

		pushd $BASE_SOURCES >/dev/null
		if [ $? -ne 0 ]; then
			echo "ERROR : can't change to directory: $BASE_SOURCES (BASE_SOURCES)"
			return 1
		fi

		wget -c -q $url
		result=$?

		popd >/dev/null

		if [ $result -ne 0 ]; then
			echo "ERROR : trouble during downloading of $name-$version ($nameversion) from $url"
			result_all=1
			break
		else
			echo "INFO  : successful downloaded of $name-$version ($nameversion) from $url to $BASE_SOURCES"
			download_library_status="$download_library_status $nameversion"
		fi
	done

	return $result_all
}

## help
help() {
	cat <<END
$0 [-h|-?]
$0 source
$0 [-D] [-X] [-B] [-n] [GeoIP|IP2Location <version>]
$0 [-A] [-n] [GeoIP|IP2Location [<specific version>]]

	source: source mode (using functions only in main script)

	-D  : download GeoIP/IP2Location source packages
	-C  : clean GeoIP/IP2Location source packages
	-X  : extract GeoIP/IP2Location source packages
	-B  : build GeoIP/IP2Location libraries
	-A  : whole chain: download/extract/build
	-n  : dry-run
	(optionally, type and version can be specified)

	-t  : GeoIP/IP2Location cross-version tests

	-h|?: this online help

	used values from environment (or defaults):
	  BASE_SOURCES=$BASE_SOURCES
	  BASE_DEVEL_GEOIP=$BASE_DEVEL_GEOIP
	  BASE_DEVEL_IP2LOCATION=$BASE_DEVEL_IP2LOCATION
END
}

if [ "$1" != "source" ]; then
	# use script not only as source (function-mode)

	#### Main
	while getopts ":DCXtFnB?h" opt; do
		case $opt in
		    'n')
			dry_run=1
			echo "NOTICE: dry-run selected"
			;;
		    'A')
			action="prepare"
			do_download="1"
			do_clean="1"
			do_extract="1"
			do_build="1"
			;;
		    'C')
			action="prepare"
			do_clean="1"
			;;
		    'D')
			action="prepare"
			do_download="1"
			;;
		    'X')
			action="prepare"
			do_extract="1"
			;;
		    'B')
			action="prepare"
			do_build="1"
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

	case $action in
	    'prepare')
		if [ "$do_download" = "1" ]; then
			if [ -z "$*" ]; then
				download_versions GeoIP || exit 1
				download_versions IP2Location || exit 1
			else
				download_versions $* || exit 1
			fi
			echo "INFO  : following libaries were successfully downloaded: $download_library_status"
		fi
		if [ "$do_clean" = "1" ]; then
			if [ -z "$*" ]; then
				clean_versions GeoIP || exit 1
				clean_versions IP2Location || exit 1
			else
				clean_versions $* || exit 1
			fi
			echo "INFO  : following libaries were successfully cleaned: $clean_library_status"
		fi
		if [ "$do_extract" = "1" ]; then
			if [ -z "$*" ]; then
				extract_versions GeoIP || exit 1
				extract_versions IP2Location || exit 1
			else
				extract_versions $* || exit 1
			fi
			echo "INFO  : following libaries were successfully extracted: $extract_library_status"
		fi
		if [ "$do_build" = "1" ]; then
			if [ -z "$*" ]; then
				build_library GeoIP || exit 1
				build_library IP2Location || exit 1
			else
				build_library $* || exit 1
			fi
			echo "INFO  : following libaries were successfully built: $build_library_status"
		fi
		;;
	    *)
		help
		;;
	esac
fi
