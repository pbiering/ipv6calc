#!/usr/bin/env bash
#
# Project    : ipv6calc
# File       : test_ipv6calc_anonymization.sh
# Copyright  : 2013-2025 by Peter Bieringer <pb (at) bieringer.de>
#
# Test ipv6calc anonymization

verbose=false
stoponerror=false
while getopts "VSh\?" opt; do
	case $opt in
	    V)
		verbose=true
		;;
	    S)
		stoponerror=true
		;;
	    *)
		echo "$0 [-V] [-S]"
		echo "    -V   verbose"
		echo "    -S   stop-on-error"
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
	if ! ./ipv6calc -v 2>&1| grep -wq "ANON_ANONYMIZE"; then
		echo "NOTICE: 'ipv6calc' has not required support for ANON_ANONYMIZE, skip standard tests"
		! $stoponerror
		return
	fi

	test="run 'ipv6calc' anonymization tests"
	echo "INFO  : $test"
	test_list | while read input filter; do
		if [ -z "$input" ]; then
			continue
		fi

		$verbose && echo "INFO: test './ipv6calc -A anonymize' for: $input"

		output="`echo "$input" | ./ipv6calc -q -A anonymize`"
		retval=$?
		if [ $retval -ne 0 ]; then
			echo "Error executing 'ipv6calc'!"
			exit 1
		fi

		$verbose && echo "INFO  : anonymized: $output"

		type_orig="`./ipv6calc -q -i "$input" -m --mrtvo IPV._TYPE`"
		if [ $? -ne 0 ]; then
			echo "ERROR  : ipv6calc result not 0: ./ipv6calc -q -i "$input" -m"
			exit 1
		fi

		type_anon="`./ipv6calc -q -i "$output" -m --mrtvo IPV._TYPE`"
		if [ $? -ne 0 ]; then
			echo "ERROR  : ipv6calc result not 0: ./ipv6calc -q -i "$output" -m"
			exit 1
		fi

		if [ -z "$type_orig" -a -z "$type_anon" ]; then
			# fallback EUI*
			type_orig="`./ipv6calc -q -i "$input"  -m --mrtvo EUI.._TYPE`"
			type_anon="`./ipv6calc -q -i "$output" -m --mrtvo EUI.._TYPE`"
		fi

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

		type_anon_compare="${type_anon_compare/anonymized-iid}"
		type_orig_compare="${type_orig_compare/anonymized-iid}"

		type_anon_compare="${type_anon_compare/anonymized-prefix}"
		type_orig_compare="${type_orig_compare/anonymized-prefix}"

		type_anon_compare="${type_anon_compare/anonymized-masked-prefix}"
		type_orig_compare="${type_orig_compare/anonymized-masked-prefix}"

		type_anon_compare="${type_anon_compare/anonymized}"
		type_orig_compare="${type_orig_compare/anonymized}"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/,+/,/g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/,+/,/g')"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/(^,|,$)//g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/(^,|,$)//g')"

		$verbose && echo "DEBUG : IPVx_TYPE orig: $type_orig_compare"
		$verbose && echo "DEBUG : IPVx_TYPE anon: $type_anon_compare"
		# Check result
		if [ "$type_orig_compare" != "$type_anon_compare" ]; then
			$verbose || echo
			echo "ERROR : IPVx_TYPE not equal for: $input (anonymized: $output)"
			echo "ERROR : IPVx_TYPE orig: $type_orig_compare"
			echo "ERROR : IPVx_TYPE anon: $type_anon_compare"
			exit 1
		else
			$verbose && echo "INFO  : result ok!" || true
		fi
		$verbose || echo -n "."
	done || return 1 
	$verbose || echo
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
			$verbose || echo
			echo "ERROR : result doesn't match on command: $command"
			echo "ERROR : result is      : $result_real"
			echo "ERROR : result expected: $result"
			exit 1
		else
			$verbose && echo "INFO  : $command -> test ok" || true
		fi
		$verbose || echo -n "."
	done || return 1
	$verbose || echo
	echo "INFO  : $test successful"
}

run_anon_options_kp_tests() {
	if ! ./ipv6calc -v 2>&1| grep -wq "ANON_KEEP-TYPE-ASN-CC"; then
		echo "NOTICE: 'ipv6calc' has not required support for Anonymization Type-ASN-CountryCode included, skip option kp tests"
		! $stoponerror
		return
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
			$verbose || echo
			echo "ERROR : result doesn't match on command: $command"
			echo "ERROR : result is      : $output"
			echo "ERROR : result expected: $result"
			exit 1
		fi
		$verbose || echo -n "."
	done || return 1
	$verbose || echo
	echo "INFO  : $test successful"

	if ! ./ipv6calc -v 2>&1| grep -wq "GeoIP"; then
		echo "NOTICE: 'ipv6calc' has not required database support for Anonymization, skip option kp tests"
		! $stoponerror
		return
	fi

	test="run 'ipv6calc' anonymization option kp TYPE/ASN/CC tests"
	echo "INFO  : $test"
	testscenarios_kp | ./ipv6calc -q -E ipv4,ipv6 | while read input result; do
		if [ -z "$input" ]; then
			continue
		fi

		$verbose && echo "INFO  : run test with: $input"

		output=$(./ipv6calc -q -A anonymize --anonymize-preset kp $input)
		if [ $? -ne 0 -o -z "$output" ]; then
			echo "ERROR : no proper result for input: $input"
			exit 1
		fi

		type_orig="`./ipv6calc -m -i -q "$input"  --mrtvo IPV._TYPE`"
		type_anon="`./ipv6calc -m -i -q "$output" --mrtvo IPV._TYPE`"

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

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/(anonymized-prefix|anonymized-iid|anonymized-masked-prefix|anonymized)//g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/(anonymized-prefix|anonymized-iid|anonymized-masked-prefix|anonymized)//g')"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/,+/,/g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/,+/,/g')"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/(^,|,$)//g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/(^,|,$)//g')"

		if [ -z "$type_orig_compare" ]; then
			$verbose || echo
			echo "ERROR : something went wrong reducing IPVx_TYPE for $input"
			exit 1
		fi
		if [ -z "$type_anon_compare" ]; then
			$verbose || echo
			echo "ERROR : something went wrong reducing IPVx_TYPE for $output"
			exit 1
		fi

		$verbose && echo "DEBUG : IPVx      orig: $input"
		$verbose && echo "DEBUG : IPVx      anon: $output"
		$verbose && echo "DEBUG : IPVx_TYPE orig: $type_orig_compare"
		$verbose && echo "DEBUG : IPVx_TYPE anon: $type_anon_compare"

		# Check result
		if [ "$type_orig_compare" != "$type_anon_compare" ]; then
			$verbose || echo
			echo "ERROR : IPVx_TYPE not equal for: $input (anonymized: $output)"
			echo "ERROR : IPVx_TYPE orig: $type_orig_compare"
			echo "ERROR : IPVx_TYPE anon: $type_anon_compare"
			exit 1
		else
			$verbose && echo "Result ok!" || true
		fi

		# Country Code (optional)
		cc_orig="`./ipv6calc -m -i -q "$input"  --mrtvo IPV._COUNTRYCODE`"
		cc_anon="`./ipv6calc -m -i -q "$output" --mrtvo IPV._COUNTRYCODE`"

		$verbose && echo "DEBUG : IPVx             orig: $input"
		$verbose && echo "DEBUG : IPVx             anon: $output"
		$verbose && echo "DEBUG : IPVx_COUNTRYCODE orig: $cc_orig"
		$verbose && echo "DEBUG : IPVx_COUNTRYCODE anon: $cc_anon"

		if [ -z "$cc_orig" -a -z "$cc_anon" ]; then
			# everything is ok, both have no CC
			true
		elif [ -z "$cc_orig" -a -n "$cc_anon" ]; then
			$verbose || echo
			echo "ERROR : IPVx_COUNTRYCODE not equal for: $input (anonymized: $output)"
			echo "ERROR : IPVx_COUNTRYCODE orig: $cc_orig"
			echo "ERROR : IPVx_COUNTRYCODE anon: $cc_anon"
			echo "ERROR : something went wrong, anon has country code while orig hasn't"
			exit 1
		elif [ -n "$cc_orig" -a -z "$cc_anon" ]; then
			$verbose || echo
			echo "ERROR : IPVx_COUNTRYCODE not equal for: $input (anonymized: $output)"
			echo "ERROR : IPVx_COUNTRYCODE orig: $cc_orig"
			echo "ERROR : IPVx_COUNTRYCODE anon: $cc_anon"
			echo "ERROR : something went wrong, orig has country code while anon hasn't"
			exit 1
		else
			# Check result
			if [ "$cc_orig" != "$cc_anon" ]; then
				$verbose || echo
				echo "ERROR : IPVx_COUNTRYCODE not equal for: $input (anonymized: $output)"
				echo "ERROR : IPVx_COUNTRYCODE orig: $cc_orig"
				echo "ERROR : IPVx_COUNTRYCODE anon: $cc_anon"
				exit 1
			else
				$verbose && echo "Result ok!" || true
			fi
		fi
		$verbose || echo -n "+"

		# ASN (optional)
		if echo "${type_orig//,/ }" | grep -Eqw "(lisp|ipv4|iid-includes-ipv4)"; then
			# skip ASN on IPv4+LISP or IID includes IPv4
			continue
		fi
		asn_orig="`./ipv6calc -m -i -q "$input"  --mrtvo IPV._AS_NUM`"
		asn_anon="`./ipv6calc -m -i -q "$output" --mrtvo IPV._AS_NUM`"

		$verbose && echo "DEBUG : IPVx     orig: $input"
		$verbose && echo "DEBUG : IPVx     anon: $output"
		$verbose && echo "DEBUG : IPVx_AS_NUM orig: $asn_orig"
		$verbose && echo "DEBUG : IPVx_AS_NUM anon: $asn_anon"

		if [ -z "$asn_orig" -a -z "$asn_anon" ]; then
			# everything is ok, both have no AS_NUM
			true
		elif [ -z "$asn_orig" -a -n "$asn_anon" ]; then
			$verbose || echo
			echo "ERROR : IPVx_AS_NUM not equal for: $input (anonymized: $output)"
			echo "ERROR : IPVx_AS_NUM orig: $asn_orig"
			echo "ERROR : IPVx_AS_NUM anon: $asn_anon"
			echo "ERROR : something went wrong, anon has AS_NUM code while orig hasn't"
			exit 1
		elif [ -n "$asn_orig" -a -z "$asn_anon" ]; then
			$verbose || echo
			echo "ERROR : IPVx_AS_NUM not equal for: $input (anonymized: $output)"
			echo "ERROR : IPVx_AS_NUM orig: $asn_orig"
			echo "ERROR : IPVx_AS_NUM anon: $asn_anon"
			echo "ERROR : something went wrong, orig has AS_NUM while anon hasn't"
			exit 1
		else
			# Check result
			if [ "$asn_orig" != "$asn_anon" ]; then
				$verbose || echo
				echo "ERROR : IPVx_AS_NUM not equal for: $input (anonymized: $output)"
				echo "ERROR : IPVx_AS_NUM orig: $asn_orig"
				echo "ERROR : IPVx_AS_NUM anon: $asn_anon"
				exit 1
			else
				$verbose && echo "Result ok!" || true
			fi
		fi
		$verbose || echo -n "."
	done || return 1
	$verbose || echo
	echo "INFO  : $test successful"
}

run_anon_options_kg_tests() {
	if ! ./ipv6calc -v 2>&1| grep -wq "ANON_KEEP-TYPE-GEONAMEID"; then
		echo "NOTICE: 'ipv6calc' has not required support for Anonymization Type-GeonameID included, skip option kg tests"
		! $stoponerror
		return
	fi

	test="run 'ipv6calc' anonymization option kg TYPE/GEONAMEID tests"
	echo "INFO  : $test"
	testscenarios_kp | ./ipv6calc -q -E ipv4,ipv6 | while read input result; do
		if [ -z "$input" ]; then
			continue
		fi

		$verbose && echo "INFO  : run test with: $input"

		output=$(./ipv6calc -q -A anonymize --anonymize-preset kg $input)
		if [ $? -ne 0 -o -z "$output" ]; then
			echo "ERROR : no proper result for input: $input"
			exit 1
		fi

		type_orig="`./ipv6calc -m -i -q "$input"  --mrtvo IPV._TYPE`"
		type_anon="`./ipv6calc -m -i -q "$output" --mrtvo IPV._TYPE`"

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

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/(anonymized-geonameid|anonymized-prefix|anonymized-iid|anonymized-masked-prefix|anonymized)//g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/(anonymized-geonameid|anonymized-prefix|anonymized-iid|anonymized-masked-prefix|anonymized)//g')"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/,+/,/g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/,+/,/g')"

		type_anon_compare="$(echo "$type_anon_compare" | perl -p -e 's/(^,|,$)//g')"
		type_orig_compare="$(echo "$type_orig_compare" | perl -p -e 's/(^,|,$)//g')"

		if [ -z "$type_orig_compare" ]; then
			$verbose || echo
			echo "ERROR : something went wrong reducing IPVx_TYPE for $input"
			exit 1
		fi
		if [ -z "$type_anon_compare" ]; then
			$verbose || echo
			echo "ERROR : something went wrong reducing IPVx_TYPE for $output"
			exit 1
		fi

		$verbose && echo "DEBUG : IPVx      orig: $input"
		$verbose && echo "DEBUG : IPVx      anon: $output"
		$verbose && echo "DEBUG : IPVx_TYPE orig: $type_orig_compare"
		$verbose && echo "DEBUG : IPVx_TYPE anon: $type_anon_compare"

		# Check result
		if [ "$type_orig_compare" != "$type_anon_compare" ]; then
			$verbose || echo
			echo "ERROR : IPVx_TYPE not equal for: $input (anonymized: $output)"
			echo "ERROR : IPVx_TYPE orig: $type_orig_compare"
			echo "ERROR : IPVx_TYPE anon: $type_anon_compare"
			exit 1
		else
			$verbose && echo "Result ok!" || true
		fi

		# Registry (only in case of LISP)
		reg_orig="`./ipv6calc -m -i -q "$input" --mrtvo IPV._REGISTRY`"
		if echo "$reg_orig" | grep -q LISP; then
			reg_orig=${reg_orig/(*} # cut lisp details

			reg_anon="`./ipv6calc -m -i -q "$output" --mrtvo IPV._REGISTRY`"

			if [ -z "$reg_orig" ]; then
				$verbose || echo
				echo "ERROR : something went wrong retrieving IPVx_REGISTRY for $input"
				exit 1
			fi
			if [ -z "$reg_anon" ]; then
				$verbose || echo
				echo "ERROR : something went wrong retrieving IPVx_REGISTRY for $output"
				exit 1
			fi

			$verbose && echo "DEBUG : IPVx          orig: $input"
			$verbose && echo "DEBUG : IPVx          anon: $output"
			$verbose && echo "DEBUG : IPVx_REGISTRY orig: $reg_orig"
			$verbose && echo "DEBUG : IPVx_REGISTRY anon: $reg_anon"

			if [ -z "$reg_orig" -a -z "$reg_anon" ]; then
				# everything is ok, both have no registry
				true
			elif [ -z "$reg_orig" -a -n "$reg_anon" ]; then
				$verbose || echo
				echo "ERROR : something went wrong, anon has registry while orig hasn't"
				exit 1
			elif [ -n "$reg_orig" -a -z "$reg_anon" ]; then
				$verbose || echo
				echo "ERROR : something went wrong, orig has registry while anon hasn't"
				exit 1
			else
				# Check result
				if [ "$reg_orig" != "$reg_anon" ]; then
					$verbose || echo
					echo "ERROR : IPVx_REGISTRY not equal for: $input (anonymized: $output)"
					echo "ERROR : IPVx_REGISTRY orig: $reg_orig"
					echo "ERROR : IPVx_REGISTRY anon: $reg_anon"
					exit 1
				else
					$verbose && echo "Result ok!" || true
				fi
			fi
		else
			# GeonameID
			gi_orig="`./ipv6calc -m -i -q "$input"  --mrtvo IPV._GEONAME_ID`"
			gi_anon="`./ipv6calc -m -i -q "$output" --mrtvo IPV._GEONAME_ID`"

			$verbose && echo "DEBUG : IPVx             orig: $input"
			$verbose && echo "DEBUG : IPVx             anon: $output"
			$verbose && echo "DEBUG : IPVx_GEONAME_ID   orig: $gi_orig"
			$verbose && echo "DEBUG : IPVx_GEONAME_ID   anon: $gi_anon"

			if [ -z "$gi_orig" -a -z "$gi_anon" ]; then
				# everything is ok, both have no CC
				true
			elif [ -z "$gi_orig" -a -n "$gi_anon" ]; then
				$verbose || echo
				echo "ERROR : IPVx_GEONAME_ID not equal for: $input (anonymized: $output)"
				echo "ERROR : IPVx_GEONAME_ID orig: $gi_orig"
				echo "ERROR : IPVx_GEONAME_ID anon: $gi_anon"
				echo "ERROR : something went wrong, anon has GeonameID while orig hasn't"
				exit 1
			elif [ -n "$gi_orig" -a -z "$gi_anon" ]; then
				$verbose || echo
				echo "ERROR : IPVx_GEONAME_ID not equal for: $input (anonymized: $output)"
				echo "ERROR : IPVx_GEONAME_ID orig: $gi_orig"
				echo "ERROR : IPVx_GEONAME_ID anon: $gi_anon"
				echo "ERROR : something went wrong, orig has GeonameID while anon hasn't"
				exit 1
			else
				# Check result
				if [ "$gi_orig" != "$gi_anon" ]; then
					$verbose || echo
					echo "ERROR : IPVx_GEONAME_ID not equal for: $input (anonymized: $output)"
					echo "ERROR : IPVx_GEONAME_ID orig: $gi_orig"
					echo "ERROR : IPVx_GEONAME_ID anon: $gi_anon"
					exit 1
				else
					$verbose && echo "Result ok!" || true
				fi
			fi

			$verbose || echo -n "+"

			# GeonameID Type
			gi_type_orig="`./ipv6calc -m -i -q "$input"  --mrtvo IPV._GEONAME_ID_TYPE`"
			gi_type_anon="`./ipv6calc -m -i -q "$output" --mrtvo IPV._GEONAME_ID_TYPE`"

			$verbose && echo "DEBUG : IPVx             orig: $input"
			$verbose && echo "DEBUG : IPVx             anon: $output"
			$verbose && echo "DEBUG : IPVx_GEONAME_ID_TYPE   orig: $gi_type_orig"
			$verbose && echo "DEBUG : IPVx_GEONAME_ID_TYPE   anon: $gi_type_anon"

			if [ -z "$gi_type_orig" -a -z "$gi_type_anon" ]; then
				# everything is ok, both have no CC
				true
			elif [ -z "$gi_type_orig" -a -n "$gi_type_anon" ]; then
				$verbose || echo
				echo "ERROR : IPVx_GEONAME_ID_TYPE not equal for: $input (anonymized: $output)"
				echo "ERROR : IPVx_GEONAME_ID_TYPE orig: $gi_type_orig"
				echo "ERROR : IPVx_GEONAME_ID_TYPE anon: $gi_type_anon"
				echo "ERROR : something went wrong, anon has GeonameID while orig hasn't"
				exit 1
			elif [ -n "$gi_type_orig" -a -z "$gi_type_anon" ]; then
				$verbose || echo
				echo "ERROR : IPVx_GEONAME_ID_TYPE not equal for: $input (anonymized: $output)"
				echo "ERROR : IPVx_GEONAME_ID_TYPE orig: $gi_type_orig"
				echo "ERROR : IPVx_GEONAME_ID_TYPE anon: $gi_type_anon"
				echo "ERROR : something went wrong, orig has GeonameID while anon hasn't"
				exit 1
			else
				# Check result
				if [ "$gi_type_orig" != "$gi_type_anon" ]; then
					$verbose || echo
					echo "ERROR : IPVx_GEONAME_ID_TYPE not equal for: $input (anonymized: $output)"
					echo "ERROR : IPVx_GEONAME_ID_TYPE orig: $gi_type_orig"
					echo "ERROR : IPVx_GEONAME_ID_TYPE anon: $gi_type_anon"
					exit 1
				else
					$verbose && echo "Result ok!" || true
				fi
			fi
		fi
		$verbose || echo -n "."
	done || return 1
	$verbose || echo
	echo "INFO  : $test successful"
}


run_anon_options_tests || exit 1
run_anon_options_kp_tests || exit 1
run_anon_options_kg_tests || exit 1
run_anon_tests || exit 1
echo "INFO  : all anonymization tests successful!"
