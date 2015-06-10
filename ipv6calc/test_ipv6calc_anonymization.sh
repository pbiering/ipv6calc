#!/bin/sh
#
# Project    : ipv6calc
# File       : test_ipv6calc_anonymization.sh
# Version    : $Id: test_ipv6calc_anonymization.sh,v 1.9 2015/06/10 05:53:57 ds6peter Exp $
# Copyright  : 2013-2015 by Peter Bieringer <pb (at) bieringer.de>
#
# Test ipv6calc anonymization

verbose=0
while getopts "Vh\?" opt; do
	case $opt in
	    V)
		verbose=1
		;;
	    *)
		echo "$0 [-V]"
		echo "    -V   verbose"
		exit 1
		;;
	esac
done

if [ ! -x ./ipv6calc ]; then
	echo "Binary './ipv6calc' missing or not executable"
	exit 1
fi

# Test Scenarios
source ./test_scenarios.sh


test_list() {
	testscenarios_filter
	testscenarios_auto_good
}

run_anon_tests() {
	test="run 'ipv6calc' anonymization tests"
	echo "INFO  : $test"
	test_list | while read input filter; do
		if [ -z "$input" ]; then
			continue
		fi

		[ "$verbose" = "1" ] && echo "INFO: test './ipv6calc -A anonymize' for: $input"

		output="`echo "$input" | ./ipv6calc -q -A anonymize`"
		retval=$?
		if [ $retval -ne 0 ]; then
			echo "Error executing 'ipv6calc'!"
			exit 1
		fi

		[ "$verbose" = "1" ] && echo "INFO  : anonymized: $output"

		type_orig="`./ipv6calc -q -i "$input" -m | grep -a "^IPV._TYPE=" | sed 's/IPV._TYPE=//'`"
		type_anon="`./ipv6calc -q -i "$output" -m | grep -a "^IPV._TYPE=" | sed 's/IPV._TYPE=//'`"

		if [ -z "$type_orig" -a -z "$type_anon" ]; then
			# fallback EUI*
			type_orig="`./ipv6calc -q -i "$input" -m | grep -a "^EUI.._TYPE=" | sed 's/EUI.._TYPE=//'`"
			type_anon="`./ipv6calc -q -i "$output" -m | grep -a "^EUI.._TYPE=" | sed 's/EUI.._TYPE=//'`"
		fi

		if [ -z "$type_orig" ]; then
			echo "ERROR : something went wrong retrieving EUIxx/IPVx_TYPE for $input"
			exit 1
		fi
		if [ -z "$type_anon" ]; then
			echo "ERROR : something went wrong retrieving EUIxx/IPVx_TYPE for $output"
			exit 1
		fi

		type_anon_compare="${type_anon/anonymized,}"
		type_orig_compare="${type_orig/anonymized,}"

		type_anon_compare="${type_anon_compare/anonymized-iid,}"
		type_orig_compare="${type_orig_compare/anonymized-iid,}"

		type_anon_compare="${type_anon_compare/anonymized-prefix,}"
		type_orig_compare="${type_orig_compare/anonymized-prefix,}"

		[ "$verbose" = "1" ] && echo "DEBUG : IPVx_TYPE orig: $type_orig_compare"
		[ "$verbose" = "1" ] && echo "DEBUG : IPVx_TYPE anon: $type_anon_compare"
		# Check result
		if [ "$type_orig_compare" != "$type_anon_compare" ]; then
			echo "ERROR : IPVx_TYPE not equal:"
			exit 1
		else
			[ "$verbose" = "1" ] && echo "INFO  : result ok!" || true
		fi
		[ "$verbose" = "1" ] || echo -n "."
	done || return 1 
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"
}

run_anon_options_tests() {
	test="run 'ipv6calc' anonymization option tests"
	echo "INFO  : $test"
	testscenarios_anonymization_options | while IFS="=" read input result; do
		if [ -z "$input" ]; then
			continue
		fi

		command="./ipv6calc -q -A anonymize $input"

		result_real="`$command`"
		if [ $? -ne 0 ]; then
			echo "ERROR : command was not proper executed: $command"
			exit 1
		fi

		if [ "$result" != "$result_real" ]; then
			echo "ERROR : result doesn't match on command: $command"
			echo "ERROR : result is      : $result_real"
			echo "ERROR : result expected: $result"
			exit 1
		else
			[ "$verbose" = "1" ] && echo "INFO  : $command -> test ok" || true
		fi
		[ "$verbose" = "1" ] || echo -n "."
	done || return 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"
}

run_anon_options_kp_tests() {
	if ! ./ipv6calc -vv 2>&1| grep -q "Country4=1 Country6=1 ASN4=1 ASN6=1"; then
		echo "NOTICE: 'ipv6calc' has not required support for Country/ASN included, skip option kp tests"
		return 0
	fi

	test="run 'ipv6calc' anonymization option kp tests"
	echo "INFO  : $test"
	testscenarios_anonymization_options_kp | while IFS="=" read input result; do
		if [ -z "$input" ]; then
			continue
		fi

		command="./ipv6calc -q -A anonymize $input"

		output="`$command`"
		if [ $? -ne 0 ]; then
			echo "ERROR : command was not proper executed: $command"
			exit 1
		fi

		if [ "$result" != "$output" ]; then
			echo "ERROR : result doesn't match on command: $command"
			echo "ERROR : result is      : $output"
			echo "ERROR : result expected: $result"
			exit 1
		fi
		[ "$verbose" = "1" ] || echo -n "."
	done || return 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"

	test="run 'ipv6calc' anonymization option kp TYPE/REGISTRY/CC tests"
	echo "INFO  : $test"
	testscenarios_kp | ./ipv6calc -q -E ipv4,ipv6 | while read input result; do
		if [ -z "$input" ]; then
			continue
		fi

		[ "$verbose" = "1" ] && echo "INFO  : run test with: $input"

		output=$(./ipv6calc -q -A anonymize --anonymize-preset kp $input)
		if [ $? -ne 0 -o -z "$output" ]; then
			echo "ERROR : no proper result for input: $input"
			exit 1
		fi

		type_orig="`./ipv6calc -m -i -q "$input"  | grep -a "^IPV._TYPE=" | sed 's/IPV._TYPE=//'`"
		type_anon="`./ipv6calc -m -i -q "$output" | grep -a "^IPV._TYPE=" | sed 's/IPV._TYPE=//'`"

		if [ -z "$type_orig" ]; then
			echo "ERROR : something went wrong retrieving IPVx_TYPE for $input"
			exit 1
		fi
		if [ -z "$type_anon" ]; then
			echo "ERROR : something went wrong retrieving IPVx_TYPE for $output"
			exit 1
		fi

		type_anon_compare="${type_anon}"
		type_orig_compare="${type_orig}"

		#echo "DEBUG : IPVx_TYPE orig not reduced: $type_orig_compare"
		#echo "DEBUG : IPVx_TYPE anon not reduced: $type_anon_compare"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/(anonymized-prefix|anonymized-iid|anonymized)//g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/(anonymized-prefix|anonymized-iid|anonymized)//g')"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/,+/,/g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/,+/,/g')"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/(^,|,$)//g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/(^,|,$)//g')"

		if [ -z "$type_orig_compare" ]; then
			echo "ERROR : something went wrong reducing IPVx_TYPE for $input"
			exit 1
		fi
		if [ -z "$type_anon_compare" ]; then
			echo "ERROR : something went wrong reducing IPVx_TYPE for $output"
			exit 1
		fi

		[ "$verbose" = "1" ] && echo "DEBUG : IPVx      orig: $input"
		[ "$verbose" = "1" ] && echo "DEBUG : IPVx      anon: $output"
		[ "$verbose" = "1" ] && echo "DEBUG : IPVx_TYPE orig: $type_orig_compare"
		[ "$verbose" = "1" ] && echo "DEBUG : IPVx_TYPE anon: $type_anon_compare"

		# Check result
		if [ "$type_orig_compare" != "$type_anon_compare" ]; then
			echo "ERROR : IPVx_TYPE not equal:"
			exit 1
		else
			[ "$verbose" = "1" ] && echo "Result ok!" || true
		fi

		# Registry
		reg_orig="`./ipv6calc -m -i -q "$input"  | grep -a "^IPV._REGISTRY=" | sed 's/IPV._REGISTRY=//'`"
		reg_anon="`./ipv6calc -m -i -q "$output" | grep -a "^IPV._REGISTRY=" | sed 's/IPV._REGISTRY=//'`"

		if [ -z "$reg_orig" ]; then
			echo "ERROR : something went wrong retrieving IPVx_REGISTRY for $input"
			exit 1
		fi
		if [ -z "$reg_anon" ]; then
			echo "ERROR : something went wrong retrieving IPVx_REGISTRY for $output"
			exit 1
		fi

		[ "$verbose" = "1" ] && echo "DEBUG : IPVx          orig: $input"
		[ "$verbose" = "1" ] && echo "DEBUG : IPVx          anon: $output"
		[ "$verbose" = "1" ] && echo "DEBUG : IPVx_REGISTRY orig: $reg_orig"
		[ "$verbose" = "1" ] && echo "DEBUG : IPVx_REGISTRY anon: $reg_anon"

		if [ -z "$reg_orig" -a -z "$reg_anon" ]; then
			# everything is ok, both have no registry
			true
		elif [ -z "$reg_orig" -a -n "$reg_anon" ]; then
			echo "ERROR : something went wrong, anon has registry while orig hasn't"
			exit 1
		elif [ -n "$reg_orig" -a -z "$reg_anon" ]; then
			echo "ERROR : something went wrong, orig has registry while anon hasn't"
			exit 1
		else
			# Check result
			if [ "$reg_orig" != "$reg_anon" ]; then
				echo "ERROR : IPVx_REGISTRY not equal:"
				exit 1
			else
				[ "$verbose" = "1" ] && echo "Result ok!" || true
			fi
		fi

		# Country Code (optional)
		cc_orig="`./ipv6calc -m -i -q "$input"  | grep -a "^IPV._COUNTRYCODE=" | sed 's/IPV._COUNTRYCODE=//'`"
		cc_anon="`./ipv6calc -m -i -q "$output" | grep -a "^IPV._COUNTRYCODE=" | sed 's/IPV._COUNTRYCODE=//'`"

		[ "$verbose" = "1" ] && echo "DEBUG : IPVx             orig: $input"
		[ "$verbose" = "1" ] && echo "DEBUG : IPVx             anon: $output"
		[ "$verbose" = "1" ] && echo "DEBUG : IPVx_COUNTRYCODE orig: $cc_orig"
		[ "$verbose" = "1" ] && echo "DEBUG : IPVx_COUNTRYCODE anon: $cc_anon"

		if [ -z "$cc_orig" -a -z "$cc_anon" ]; then
			# everything is ok, both have no CC
			true
		elif [ -z "$cc_orig" -a -n "$cc_anon" ]; then
			echo "ERROR : something went wrong, anon has country code while orig hasn't"
			exit 1
		elif [ -n "$cc_orig" -a -z "$cc_anon" ]; then
			echo "ERROR : something went wrong, orig has country code while anon hasn't"
			exit 1
		else
			# Check result
			if [ "$cc_orig" != "$cc_anon" ]; then
				echo "ERROR : IPVx_COUNTRYCODE not equal:"
				exit 1
			else
				[ "$verbose" = "1" ] && echo "Result ok!" || true
			fi
		fi
		[ "$verbose" = "1" ] || echo -n "."
	done || return 1
	[ "$verbose" = "1" ] || echo
	echo "INFO  : $test successful"
}


run_anon_options_tests || exit 1
run_anon_options_kp_tests || exit 1
run_anon_tests || exit 1
echo "INFO  : all anonymization tests successful!"
