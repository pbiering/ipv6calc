#!/bin/sh
#
# Project    : ipv6calc
# File       : test_ipv6calc_anonymization.sh
# Version    : $Id: test_ipv6calc_anonymization.sh,v 1.4 2014/04/22 20:29:19 ds6peter Exp $
# Copyright  : 2013-2013 by Peter Bieringer <pb (at) bieringer.de>
#
# Test ipv6calc anonymization

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
	echo "Run 'ipv6calc' anonymization tests..."
	test_list | while read input filter; do
		if [ -z "$input" ]; then
			# end
			continue
		fi

		echo "Test './ipv6calc -A anonymize' for: $input"

		output="`echo "$input" | ./ipv6calc -q -A anonymize`"
		retval=$?
		if [ $retval -ne 0 ]; then
			echo "Error executing 'ipv6calc'!"
			exit 1
		fi

		echo "INFO  : anonymized: $output"

		type_orig="`./ipv6calc -q -i "$input" -m | grep "^IPV._TYPE=" | sed 's/IPV._TYPE=//'`"
		type_anon="`./ipv6calc -q -i "$output" -m | grep "^IPV._TYPE=" | sed 's/IPV._TYPE=//'`"

		if [ -z "$type_orig" -a -z "$type_anon" ]; then
			# fallback EUI*
			type_orig="`./ipv6calc -q -i "$input" -m | grep "^EUI.._TYPE=" | sed 's/EUI.._TYPE=//'`"
			type_anon="`./ipv6calc -q -i "$output" -m | grep "^EUI.._TYPE=" | sed 's/EUI.._TYPE=//'`"
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

		echo "DEBUG : IPVx_TYPE orig: $type_orig_compare"
		echo "DEBUG : IPVx_TYPE anon: $type_anon_compare"
		# Check result
		if [ "$type_orig_compare" != "$type_anon_compare" ]; then
			echo "ERROR : IPVx_TYPE not equal:"
			exit 1
		else
			echo "Result ok!"
		fi
	done || return 1 
}

run_anon_options_tests() {
	echo "Run 'ipv6calc' anonymization option tests..."
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
			echo "INFO  : $command -> test ok"
		fi
	done || return 1
}

testscenarios_kp() {
	testscenarios_anonymization_options_kp | while IFS="=" read input result; do
		echo "$input" | awk '{ print $NF }'
	done

	test_list | ./ipv6calc -E ipv4,ipv6 | while read input filter; do
		echo "$input"
	done
}

run_anon_options_kp_tests() {
	if ! ./ipv6calc -vv 2>&1| grep -q "Country4=1 Country6=1 ASN4=1 ASN6=1"; then
		echo "NOTICE 'ipv6calc' has not required support for Country/ASN included, skip option kp tests..."
		return 0
	fi

	echo "Run 'ipv6calc' anonymization option kp tests..."
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
	done

	echo "Run 'ipv6calc' anonymization option kp TYPE tests..."
	testscenarios_kp | while read input result; do
		output=$(./ipv6calc -q -A anonymize --anonymize-preset kp $input)

		type_orig="`./ipv6calc -m -i -q "$input"  | grep "^IPV._TYPE=" | sed 's/IPV._TYPE=//'`"
		type_anon="`./ipv6calc -m -i -q "$output" | grep "^IPV._TYPE=" | sed 's/IPV._TYPE=//'`"

		if [ -z "$type_orig" ]; then
			echo "ERROR : something went wrong retrieving EUIxx/IPVx_TYPE for $input"
			exit 1
		fi
		if [ -z "$type_anon" ]; then
			echo "ERROR : something went wrong retrieving EUIxx/IPVx_TYPE for $output"
			exit 1
		fi

		type_anon_compare="${type_anon}"
		type_orig_compare="${type_orig}"

		echo "DEBUG : IPVx_TYPE orig not reduced: $type_orig_compare"
		echo "DEBUG : IPVx_TYPE anon not reduced: $type_anon_compare"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/(anonymized-prefix|anonymized-iid|anonymized)//g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/(anonymized-prefix|anonymized-iid|anonymized)//g')"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/,+/,/g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/,+/,/g')"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/,$//g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/,$//g')"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/^,//g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/^,//g')"

		echo "DEBUG : IPVx      orig: $input"
		echo "DEBUG : IPVx      anon: $output"
		echo "DEBUG : IPVx_TYPE orig: $type_orig_compare"
		echo "DEBUG : IPVx_TYPE anon: $type_anon_compare"
		# Check result
		if [ "$type_orig_compare" != "$type_anon_compare" ]; then
			echo "ERROR : IPVx_TYPE not equal:"
			exit 1
		else
			echo "Result ok!"
		fi

	done || return 1
}


run_anon_options_tests || exit 1
run_anon_options_kp_tests || exit 1
run_anon_tests || exit 1
