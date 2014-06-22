#!/bin/bash
#
# Project    : ipv6calc
# File       : autogen-all-variants.sh
# Version    : $Id: autogen-all-variants.sh,v 1.36 2014/06/22 09:49:25 ds6peter Exp $
# Copyright  : 2011-2014 by Peter Bieringer <pb (at) bieringer.de>
#
# Information: run autogen.sh with all supported variants
#
# can run also through various version of GeoIP (-g) and IP2Location (-i) libraries
#  see autogen-support.sh for details

status_file="autogen-all-variants.status"

## Generate configure variants
autogen_variants() {
	if [ "$skip_main_test" != "1" ]; then
		cat <<END | grep -v ^#
NONE#
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

	if [ "$ip2location_versions_test" = "1" ]; then
		for version in $ip2location_versions; do
			local testlist=""
			for version_test in $ip2location_versions; do
				if ip2location_cross_version_test_blacklist $version $version_test; then
					testlist="$testlist I:$version_test"
				fi
			done
			echo "IP2LOCATION#--enable-ip2location $(options_from_name_version IP2Location $version)#$testlist"
		done
	fi

	if [ "$geoip_versions_test" = "1" ]; then
		for version in $geoip_versions; do
			local testlist=""
			for version_test in $geoip_versions; do
				if geoip_cross_version_test_blacklist $version $version_test; then
					testlist="$testlist G:$version_test"
				fi
			done
			echo "GEOIP#--enable-geoip $(options_from_name_version GeoIP $version)#$testlist"
		done
	fi
}

help() {
	cat <<END
$0
	-h|-?	this online help
	-f	force new run, remove status file unconditionally
	-r	force re-run, after finished one, remove status file
	-W	add option -W (warning) to autogen.sh
	-N	add --no-static-build to autogen.sh
	-I	skip IP2Location builds
	-g	run through internal defined GeoIP versions
	-i	run through internal defined IP2Location versions
	-M	skip main tests
	-n	dry-run, show only what would be build
END
}


while getopts ":cNMigrIfWn?h" opt; do
	case $opt in
	    'f')
		force=1
		;;
	    'r')
		rerun=1
		;;
	    'W')
		options_add="$options_add -W"
		;;
	    'N')
		no_static_build=1
		;;
	    'n')
		dry_run=1
		;;
	    'M')
		skip_main_test=1
		;;
	    'I')
		skip_token="IP2LOCATION"
		;;
	    'g')
		geoip_versions_test="1"
		skip_shared="1"
		no_static_build=1
		;;
	    'i')
		ip2location_versions_test="1"
		skip_shared="1"
		no_static_build=1
		;;
	    'c')
		cross_versions_test="1"
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

source ./autogen-support.sh "source"

if [ -n "$options_add" ]; then
	echo "INFO  : additional options: $options_add"
fi

if [ -f "$status_file" -a "$dry_run" != "1" ]; then
	echo "INFO  : status file found: $status_file"

	if [ "$force" = "1" ]; then
		echo "NOTICE: remove status file (force)"
		rm $status_file
	else
		if grep -q ":END:" $status_file; then
			if [ "$rerun" = "1" ]; then
				echo "NOTICE: all runs successful, option -r given, status file removed (re-run)"
				rm $status_file
			else
				echo "NOTICE: all runs successful, nothing more to do (use -r for force a re-run)"
				exit 0
			fi
		else
			if [ "$rerun" = "1" ]; then
				echo "NOTICE: option -r for forcing a re-run is useless, last run was not finished (use -f)"
				exit 0
			fi
		fi
	fi
fi

if [ "$dry_run" != "1" ]; then
	if [ ! -f "$status_file" ]; then
		echo "INFO  : status file missing, create: $status_file"
		date "+%s:START:" >$status_file
	fi
fi

IONICE="ionice -c 3"

if ! $IONICE true; then
	echo "NOTICE: disable use of ionice, not supported"
	IONICE=""
fi

# variants
for liboption in "normal" "shared"; do
	if [ "$skip_shared" = "1" -a "$liboption" = "shared" ]; then
		continue
	fi

	autogen_variants | while IFS="#" read token buildoptions testlist; do
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
		if [ -f "$status_file" ]; then
			if egrep -q ":FINISHED:variants:$options:" $status_file; then
				echo "NOTICE : skip variant run with: $options"
				continue
			fi
		fi

		if echo "$token" | egrep -wq "$skip_token"; then
			echo "NOTICE : skip variant because of token: $token"
			if [ "$dry_run" = "1" ]; then
				date "+%s:FINISHED:variants:$options:SKIPPED" >>$status_file
			fi
			continue
		fi

		if [ "$dry_run" = "1" ]; then
			if [ -n "$testlist" ]; then
				echo "INFO  : would call(dry-run): ./autogen.sh $options (testlist: $testlist)"
			else
				echo "INFO  : would call(dry-run): ./autogen.sh $options"
			fi
		else
			# run autogen
			echo "INFO  : call: ./autogen.sh $options"

			nice -n 20 $IONICE ./autogen.sh $options
			if [ $? -ne 0 ]; then
				echo "ERROR : autogen.sh reports an error with options: $options"
				exit 1
			fi

			if [ -n "$testlist" ]; then
				for entry in $testlist; do
					name=${testlist/:*}
					version=${testlist/*:}
					lib=$(options_from_name_version $name $version "only-lib")

					echo "INFO  : call: LD_PRELOAD=$lib make test"
					LD_PRELOAD="$lib" make test
					if [ $? -ne 0 ]; then
						echo "ERROR : autogen.sh reports an error with options: $options during testlist entry: $entry"
						exit 1
					fi
					date "+%s:FINISHED:variants:$options:TESTLIST-ENTRY=$entry" >>$status_file
				done
			fi

			# add entry in log
			date "+%s:FINISHED:variants:$options:OK" >>$status_file
		fi
	done || exit 1
done


if [ "$dry_run" != "1" ]; then
	echo "INFO  : congratulations, all variants built successful!"
	date "+%s:END:" >>$status_file
	cat $status_file

	make autoclean >/dev/null 
	if [ $? -ne 0 ]; then
		echo "ERROR : 'make autoclean' failed"
		exit 1
	fi
fi
