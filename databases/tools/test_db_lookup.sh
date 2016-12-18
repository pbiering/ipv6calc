#!/bin/bash
#
# Project    : ipv6calc
# File       : test_db_lookup.sh
# Version    : $Id$
# Copyright  : 2014-2014 by Peter Bieringer <pb (at) bieringer.de>
#
# Test raw database entries against ipv6calc output (to check, whether BuiltIn databases are correctly aggregated)
#
# Execution only required after lookup code changes

renice -n 19 -p $$
ionice -c idle -p $$


ipv6calc="../../ipv6calc/ipv6calc"

test_ipv4_registry() {
	for registry in arin ripencc apnic afrinic lacnic; do
		case $registry in
		    arin)
			registry_file="delegated-$registry-extended-latest"
			;;
		    *)
			registry_file="delegated-$registry-latest"
			;;
		esac

		registry_file="../registries/$registry/$registry_file"

		if [ ! -f "$registry_file" ]; then
			echo "WARN  : no registry database file: $registry_file (=> skip test)"
			continue
		fi

		num="`cat "$registry_file" | awk -F'|' '{ if (($3 == "ipv4") && ($2 != "*")) print $4; }' | wc -l`"

		echo "INFO  : test registry: $registry ($registry_file) entries=$num"

		ipv6calc_output_expected="`echo "IPV4_REGISTRY=$registry" | tr a-z A-Z`"

		echo "DEBUG : expected output: $ipv6calc_output_expected"

		counter=0
		interval=$[ $num / 100 + 1 ]
		if [ $interval -le 1 ]; then
			interval=$[ $num / 10 + 1 ]
		fi
		trigger=$interval

		cat "$registry_file" | awk -F'|' '{ if (($3 == "ipv4") && ($2 != "*")) print $4 " " $5; }' | while read ipv4 amount; do
			counter=$[ $counter + 1 ]

			last=$[ amount - 1 ]

			last_0=$[ last / 16777216 ]
			last=$[ last - $last_0 * 16777216 ]

			last_1=$[ last  / 65536 ]
			last=$[ last - $last_1 * 65536 ]

			last_2=$[ last / 256 ]

			last_3=$[ last - $last_2 * 256 ]

			#echo "DEBUG : amount to add: $last_0 $last_1 $last_2 $last_3"

			ipv6_0="`echo "$ipv4" | awk -F. '{ print $1 }'`"
			ipv6_1="`echo "$ipv4" | awk -F. '{ print $2 }'`"
			ipv6_2="`echo "$ipv4" | awk -F. '{ print $3 }'`"
			ipv6_3="`echo "$ipv4" | awk -F. '{ print $4 }'`"

			# add ipv4 + amount - 1 to get last IPv4 address

			result_3=$[ $last_3 + ipv6_3 ]
			if [ $result_3 -gt 255 ]; then
				ipv6_2=$[ $ipv6_2 + 1 ]
				result_3=$[ $result_3 - 256 ]
			fi

			result_2=$[ $last_2 + ipv6_2 ]
			if [ $result_2 -gt 255 ]; then
				ipv6_1=$[ $ipv6_1 + 1 ]
				result_2=$[ $result_2 - 256 ]
			fi

			result_1=$[ $last_1 + ipv6_1 ]
			if [ $result_1 -gt 255 ]; then
				ipv6_0=$[ $ipv6_0 + 1 ]
				result_1=$[ $result_1 - 256 ]
			fi

			result_0=$[ $last_0 + ipv6_0 ]
			if [ $result_0 -gt 255 ]; then
				echo "ERROR : add engine overflow!"
				exit 1
			fi

			ipv4_last="$result_0.$result_1.$result_2.$result_3"

			#echo "DEBUG : test registry=$registry ipv4=$ipv4 amount=$amount (ipv4_last=$ipv4_last) counter=$counter"

			for ip in $ipv4 $ipv4_last; do
				ipv6calc_output="`$ipv6calc -q -m -i $ip | grep "^IPV4_REGISTRY="`"

				if [ -z "$ipv6calc_output" ]; then
					echo "ERROR : ipv6calc output empty, no IPV4_REGISTRY line (registry=$registry ipv4=$ipv4)"
					exit 1
				fi

				if [ "$ipv6calc_output" != "$ipv6calc_output_expected" ]; then
					echo "ERROR : output >$ipv6calc_output< not matching expected >$ipv6calc_output_expected< ($ip)"
					exit 1
				fi
			done

			if [ $counter -eq $trigger ]; then
				percent=$[ $counter * 100 / $num ]
				echo "INFO  : registry=$registry already tested: $counter/$num $percent% ($ipv4 - $ipv4_last)"
				trigger=$[ $trigger + $interval ]
			fi
		done || return 1
	done || return 1
}

test_asn_registry() {
	for registry in arin ripencc apnic afrinic lacnic; do
		case $registry in
		    arin)
			registry_file="delegated-$registry-extended-latest"
			;;
		    *)
			registry_file="delegated-$registry-latest"
			;;
		esac

		registry_file="../registries/$registry/$registry_file"

		if [ ! -f "$registry_file" ]; then
			echo "WARN  : no registry database file: $registry_file (=> skip test)"
			continue
		fi

		num="`cat "$registry_file" | awk -F'|' '{ if (($3 == "asn") && ($2 != "*")) print $4; }' | wc -l`"

		echo "INFO  : test registry: $registry ($registry_file) entries=$num"

		ipv6calc_output_expected="`echo "AS_NUM_REGISTRY=$registry" | tr a-z A-Z`"

		echo "DEBUG : expected output: $ipv6calc_output_expected"

		counter=0
		interval=$[ $num / 100 + 1 ]
		if [ $interval -le 1 ]; then
			interval=$[ $num / 10 + 1 ]
		fi
		trigger=$interval

		cat "$registry_file" | awk -F'|' '{ if (($3 == "asn") && ($2 != "*")) print $4; }' | while read asn; do
			counter=$[ $counter + 1 ]

			#echo "DEBUG : test registry=$registry asn=$asn counter=$counter"

			ipv6calc_output="`$ipv6calc -q -m -I asn -i $asn | grep "^AS_NUM_REGISTRY="`"

			if [ -z "$ipv6calc_output" ]; then
				echo "ERROR : ipv6calc output empty, no AS_NUM_REGISTRY line (registry=$registry asn=$asn)"
				exit 1
			fi

			if [ "$ipv6calc_output" != "$ipv6calc_output_expected" ]; then
				echo "ERROR : output >$ipv6calc_output< not matching expected >$ipv6calc_output_expected< ($asn)"
				#exit 1
			fi

			if [ $counter -eq $trigger ]; then
				percent=$[ $counter * 100 / $num ]
				#echo "INFO  : registry=$registry already tested: $counter/$num $percent% ($asn)"
				trigger=$[ $trigger + $interval ]
			fi
		done || return 1
	done || return 1
}

help() {
	echo "-a  : test_asn_registry"
	echo "-4  : test_ipv4_registry"
}

if [ -z "$1" ]; then
	help
	exit 0
fi

while getopts ":a4" opt; do
        case $opt in
            a)
		test_asn=1
                ;;
            4)
		test_ipv4=1
                ;;
            \?|h)
                echo "Invalid option: -$OPTARG" >&2
		help
		exit 0
                ;;
        esac
done

shift $[ $OPTIND - 1 ]

if [ "$test_asn"  = "1" ]; then
	test_asn_registry  || exit 1
fi

if [ "$test_ipv4" = "1" ]; then
	test_ipv4_registry || exit 1
fi

echo "INFO  : all defined database tests successful"
