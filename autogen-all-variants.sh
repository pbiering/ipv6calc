#!/bin/bash
#
# Project    : ipv6calc
# File       : autogen-all-variants.sh
# Version    : $Id$
# Copyright  : 2011-2021 by Peter Bieringer <pb (at) bieringer.de>
#
# Information: run autogen.sh with all supported variants
#
# history: can run also through various version of GeoIP (-g) and IP2Location (-i) libraries
#  see autogen-support.sh for details

status_file="autogen-all-variants.status"

## Generate configure variants
autogen_variants() {
	autogen_variants_list
	autogen_variants_list | while read line; do
		echo "${line:+$line }--clang"
	done

	autogen_variants_list | while read line; do
		echo "${line:+$line }--gcc-Os"
	done

	# skip IP2Location and MaxMindDB based
	autogen_variants_list | egrep -vw "(IP2LOCATION)" | while read line; do
		echo "${line:+$line }--m32"
	done
	autogen_variants_list | egrep -vw "(IP2LOCATION)" | while read line; do
		echo "${line:+$line }--clang --m32"
	done
}

autogen_variants_list() {
	if [ "$skip_main_test" != "1" ]; then
		cat <<END | grep -v ^#
NONE#
NONE#--enable-bundled-md5 --enable-bundled-getopt
IP2LOCATION#-i
IP2LOCATION#-i --ip2location-dyn
GEOIP2 DBIP2#-m
GEOIP2 DBIP2#-m --mmdb-dyn
GEOIP2#-m --disable-dbip2
GEOIP2#-M --disable-dbip2
DBIP2#-m --disable-geoip2
DBIP2#-M --disable-geoip2
IP2LOCATION GEOIP DBIP GEOIP2 DBIP2#-a
IP2LOCATION GEOIP DBIP GEOIP2 DBIP2#-A
NONE#--disable-db-ieee
NONE#--disable-db-ipv4
NONE#--disable-db-ipv6
NONE#--disable-db-ipv6 --disable-db-ipv4
NONE#--disable-db-ipv6 --disable-db-ipv4 --disable-db-ieee
NONE#--disable-db-ipv6 --disable-db-ipv4 --disable-db-ieee --disable-as-registry
NONE#--disable-db-ipv6 --disable-db-ipv4 --disable-db-ieee --disable-cc-registry
NONE#--disable-db-ipv6 --disable-db-ipv4 --disable-db-ieee --disable-as-registry --disable-cc-registry
NONE#--disable-db-ipv6 --disable-db-ieee
NONE#--disable-db-ipv4 --disable-db-ieee
NONE#--disable-db-builtin
END
	fi

	if [ "$ip2location_versions_test" = "1" ]; then
		for version in $ip2location_versions; do
			[ ${version:0:1} = "!" ] && continue
			local testlist=""
			local option=""
			for version_test in $ip2location_versions; do
				if [ "$dynamic_load" = "1" ]; then
					option="--ip2location-dyn"
					# unconditionally test all versions
					testlist="$testlist I:$version_test"
				else
					[ ${version_test:0:1} = "!" ] && continue
					if ip2location_cross_version_test_blacklist $version $version_test; then
						testlist="$testlist I:$version_test"
					fi
				fi
			done
			echo "NONE#--enable-ip2location $option $(options_from_name_version IP2Location $version)#$testlist"
		done
	fi

	if [ "$geoip_versions_test" = "1" ]; then
		for version in $geoip_versions; do
			[ ${version:0:1} = "!" ] && continue
			local testlist=""
			local option=""
			for version_test in $geoip_versions; do
				if [ "$dynamic_load" = "1" ]; then
					option="--geoip-dyn"
					# unconditionally test all versions
					testlist="$testlist G:$version_test"
				else
					[ ${version_test:0:1} = "!" ] && continue
					if geoip_cross_version_test_blacklist $version $version_test; then
						testlist="$testlist G:$version_test"
					fi
				fi
			done
			echo "NONE#--enable-geoip $option $(options_from_name_version GeoIP $version)#$testlist"
		done
	fi
}

help() {
	cat <<END
$0
	-h|-?	this online help
	-f	force new run, remove status file unconditionally
	-r	force re-run, after finished one, remove status file
	-N	add --no-static-build to autogen.sh
	-I	skip IP2Location builds using system wide available library
	-G	skip GeoIP builds using system wide available library
	-g	run through internal defined GeoIP versions
	-i	run through internal defined IP2Location versions
	-D	enable dynamic library loading in run through versions	
	-M	skip main tests
	-n	dry-run, show only what would be build
	-c	cross version test
	-b	batch test (do not stop on error)
END
}


while getopts ":cbDNMigrIGfWn?h" opt; do
	case $opt in
	    'b')
		batch=1
		;;
	    'f')
		force=1
		;;
	    'r')
		rerun=1
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
	    'G')
		skip_token="GEOIP"
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
	    'D')
		dynamic_load="1"
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
		date "+%s:START:${batch:+BATCHMODE}" >$status_file
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
		buildoptions=$(echo $buildoptions)
		if [ -n "$options_add" ]; then
			if [ "$no_static_build" = "1" ]; then
				options="--no-static-build $options_add $buildoptions"
			else
				options="${options_add:+$options_add }$buildoptions"
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
			options="${options:+$options }-S"
			;;
		esac

		# extend options in fallback case
		if [ -n "$ip2location_options_extra" ]; then
			if echo "$token" | egrep -wq "IP2LOCATION"; then
				options="${options:+$options }$ip2location_options_extra"
			fi
		fi

		if [ -n "$geoip_options_extra" ]; then
			if echo "$token" | egrep -wq "GEOIP"; then
				options="${options:+$options }$geoip_options_extra"
			fi
		fi

		# check for already executed option combination
		if [ -f "$status_file" ]; then
			if [ -z "$testlist" ]; then
				if egrep -q ":FINISHED:variants:$options:" $status_file; then
					echo "NOTICE: skip variant run (already finished) with: $options"
					continue
				fi
			else
				echo "NOTICE: testlist not empty, check dedicated build (testlist: $testlist) with: $options"
			fi
		fi

		if echo "$token" | egrep -wq "$skip_token"; then
			echo "NOTICE: skip variant because of token: $token"
			if [ "$dry_run" = "1" ]; then
				date "+%s:FINISHED:variants:$options:SKIPPED" >>$status_file
			fi
			continue
		fi

		options_test=""

		if [ -n "$testlist" ]; then
			options_test="--no-test"
		fi

		if [ "$dry_run" = "1" ]; then
			echo "INFO  : would call(dry-run): ./autogen.sh $options_test $options"
			if [ -z "$testlist" ]; then
				continue
			fi
		else
			# run autogen
			echo "INFO  : call: ./autogen.sh $options_test $options"

			nice -n 20 $IONICE ./autogen.sh $options_test $options
			if [ $? -ne 0 ]; then
				echo "ERROR : autogen.sh reports an error with options: $options_test $options"
				if [ "$batch" = "1" ]; then
					date "+%s:BROKEN:variants:$options" >>$status_file
				else
					exit 1
				fi
			else
				# add entry in log
				date "+%s:FINISHED:variants:$options:OK" >>$status_file
			fi
		fi

		if [ -n "$testlist" ]; then
			for entry in $testlist; do
				if egrep -q ":FINISHED:variants:$options:TESTLIST-ENTRY=$entry" $status_file; then
					echo "NOTICE: skip variant test (already finished) with: $options and $entry"
					continue
				fi

				if [ "$dry_run" = "1" ]; then
					echo "INFO  : would call(dry-run): ./autogen.sh $options (testlist entry: $entry)"
					continue
				fi

				name=${entry/:*}
				version=${entry/*:}
				lib=$(options_from_name_version $name $version "only-lib")
				libdir=$(options_from_name_version $name $version "only-libdir")

				if [ -z "$lib" ]; then
					echo "ERROR : something wrong in call of: options_from_name_version $name $version 'only-lib'"
					exit 1
				fi

				if [ -z "$libdir" ]; then
					echo "ERROR : something wrong in call of: options_from_name_version $name $version 'only-libdir'"
					exit 1
				fi

				if [ ! -e "$lib" ]; then
					echo "ERROR : library missing: $lib (got from: options_from_name_version $name $version 'only-lib')"
					exit 1
				fi

				if [ ! -d "$libdir" ]; then
					echo "ERROR : library directory missing: $libdir (got from: options_from_name_version $name $version 'only-libdir')"
					exit 1
				fi

				if [ "$dynamic_load" = "1" ]; then
					case $name in
					    I*)
						feature_string="IP2Location"
						;;
					    G*)
						feature_string="GeoIP"
						;;
					esac

					# check for feature dynamic load
					echo "INFO  : call: LD_LIBRARY_PATH=$libdir ./ipv6calc/ipv6calc -v"
					if ! LD_LIBRARY_PATH="$libdir" ./ipv6calc/ipv6calc -v 2>&1 | grep version | grep -qw $feature_string; then
						echo "ERROR : call has not included required feature string '$feature_string': LD_LIBRARY_PATH=$libdir ./ipv6calc/ipv6calc -v"
						exit 1
					fi

					echo "INFO  : call: LD_LIBRARY_PATH=$libdir make test-ldlibpath"
					LD_LIBRARY_PATH="$libdir" make test-ldlibpath
					result=$?
				else
					echo "INFO  : call: LD_PRELOAD=$lib make test-ldlibpath"
					LD_LIBRARY_PATH="$libdir" make test-ldlibpath
					result=$?
				fi

				if [ $result -ne 0 ]; then
					echo "ERROR : autogen.sh reports an error with options: $options during testlist entry: $entry"
					if [ "$dynamic_load" = "1" ]; then
						echo "NOTICE: executed command: LD_LIBRARY_PATH=$libdir make test-ldlibpath"
					else
						echo "NOTICE: executed command: LD_PRELOAD="$lib" make test-ldlibpath"
					fi

					if [ "$batch" = "1" ]; then
						date "+%s:BROKEN:variants:$options:TESTLIST-ENTRY=$entry" >>$status_file
					else
						exit 1
					fi
				else
					date "+%s:FINISHED:variants:$options:TESTLIST-ENTRY=$entry" >>$status_file
				fi
			done
		fi

	done || exit 1
done


if [ "$dry_run" != "1" ]; then
	if grep -q ":BROKEN:" $status_file; then
		echo "ERROR : there are BROKEN builds:"
		grep ":BROKEN:" $status_file
		exit 1
	else
		echo "INFO  : congratulations, all variants built successful!"
	fi
	date "+%s:END:" >>$status_file
	cat $status_file

	make autoclean >/dev/null 
	if [ $? -ne 0 ]; then
		echo "ERROR : 'make autoclean' failed"
		exit 1
	fi
fi
