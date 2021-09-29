#!/bin/bash
#
# Project    : ipv6calc
# File       : ipv6calc-create-update-ipset.sh
# Version    : $Id$
# Copyright  : 2021-2021 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Shell script to manage country code based "ipset" lists
#
# requires sudo support for 'iplist' like
#
# /etc/sudoers.d/99-ipv6calc
# poweruser     ALL=NOPASSWD:/usr/sbin/ipset
#
#
# 20210929/bie: initial version


prefix="Net"

help() {
	cat <<END
Usage: $(basename "$0") -C <CountryCode> [-P <SETNAME-Prefix>] [-L <list>] [-h|?] [-d] [-q] [-t] [-n]
	-q			more quiet (default if called by cron)
	-C <CountryCode>	Country Code for database filter
	-P <SETNAME-Prefix>	Prefix for 'ipset' SETNAME (optional, default: $prefix)
	-L <list>>		Selected list (IPv4|IPv6|IPv6to4) (optional, default: ALL)
	-d			debug
	-n			no dry-run
	-t			trace
	-h|?			this online help

honors environment
	IPV6CALC_BIN		'ipv6calc' binary

'iplist' setname will be named: Prefix_CountryCode_(IPv4|IPv6|IPv6to4)
END
}

## default
dryrun=true
debug=false
trace=false

if [ -t 0 ]; then
	quiet=false
else
	quiet=true
fi

## parse options
while getopts "\?hdnqtC:P:L:" opt; do
	case $opt in
	    P)
		prefix="$OPTARG"
		;;
	    C)
		countrycode="$OPTARG"
		;;
	    L)
		listselected="$OPTARG"
		;;
	    d)
		debug=true
		;;
	    n)
		dryrun=false
		;;
	    q)
		quiet=true
		;;
	    t)
		trace=true
		;;
	    \?|h)
		help
		exit 1
		;;
	    *)
		echo "Invalid option: -$OPTARG" >&2
		exit 0
		;;
	esac
done

if ! $quiet; then
	$debug  && echo "DEBUG : debug  mode enabled"
	$dryrun && echo "NOTICE: dryrun mode enabled"
	$trace  && echo "DEBUG : trace  mode enabled"
fi

## failsafe checks

# mandatory options
if [ -z "$countrycode" ]; then
	echo "ERROR : mandatory option missing: '-C <CountryCode>'"
	exit 1
fi

# ipv6calc binary
if [ -n "${IPV6CALC_BIN}" ]; then
	if [ ! -e "${IPV6CALC_BIN}" ]; then
		echo "ERROR : provided IPV6CALC_BIN is not existing: $IPV6CALC_BIN"
		exit 1
	fi
	if [ ! -x "${IPV6CALC_BIN}" ]; then
		echo "ERROR : provided IPV6CALC_BIN is not executable: $IPV6CALC_BIN"
		exit 1
	fi

	ipv6calc="$IPV6CALC_BIN"
	$quiet || echo "INFO  : provided via IPV6CALC_BIN: $ipv6calc"
else
	ipv6calc="$(which 'ipv6calc' 2>/dev/null)"
	if [ -z "$ipv6calc" ]; then
		echo "ERROR : no 'ipv6calc' binary found via system PATH"
		exit 1
	fi
	$quiet || echo "INFO  : found in system PATH: $ipv6calc"
fi


## get features
ipv6calc_features=$($ipv6calc -v 2>&1)

declare -A ipv6calc_support

if [[ $ipv6calc_features =~ DB_IPV4_DUMP_CC ]]; then
	$quiet || echo "INFO  : $ipv6calc supports DB_IPV4_DUMP_CC"
	ipv6calc_support[IPv4]=1
	ipv6calc_support[IPv6to4]=1
fi

if [[ $ipv6calc_features =~ DB_IPV6_DUMP_CC ]]; then
	$quiet || echo "INFO  : $ipv6calc supports DB_IPV6_DUMP_CC"
	ipv6calc_support[IPv6]=1
fi

if [ ${#ipv6calc_support[@]} -eq 0 ]; then
	echo "ERROR : no DB_IPV*_DUMP_CC support found in: $ipv6calc"
	exit 1
fi

## functions
ipset_list_members() {
	local setname="$1"

	$debug && echo "DEBUG : called: ipset_list_members $setname" >&2

	# note: ipset will always print header, this is currently not deselectable
	sudo ipset list $setname -o xml | while read line; do
		$trace && echo "DEBUG : ipset_list_members: $line (TEST)" >&2
		if [[ $line =~ \<member\>\<elem\>([0-9a-f:./]+)\<\/elem\> ]]; then
			$trace && echo "DEBUG : ipset_list_members: $line (MATCH)" >&2
			echo "${BASH_REMATCH[1]}"
		fi
	done
}


## main work
declare -A summary
require_no_dryrun=false
result=0

for list in ${!ipv6calc_support[@]}; do
	if [ -n "$listselected" -a "$list" != "$listselected" ]; then
		$quiet || echo "INFO  : skip not selected list for country code: $list ($countrycode)"
		continue
	fi

	$quiet || echo "INFO  : process list for country code: $list ($countrycode)"

	case $list in
	    IPv4)
		ipv6calc_options="-A dbdump -E ipv4.db.cc=$countrycode"
		family="inet"
		;;
	    IPv6to4)
		ipv6calc_options="-A dbdump -E ipv4.db.cc=$countrycode -O ipv6to4"
		family="inet6"
		;;
	    IPv6)
		ipv6calc_options="-A dbdump -E ipv6.db.cc=$countrycode"
		family="inet6"
		;;
	    *)
		echo "ERROR : unsupported list: $list (FIX-CODE)"
		exit 1
		;;
	esac

	unset list_entries
	unset list_entries_array
	declare -A list_entries
	declare -a list_entries_array
	list_entries_array=( $($ipv6calc -q $ipv6calc_options) )
	rc=$?

	if [ $rc -ne 0 ]; then
		echo "ERROR : execution not successful: $ipv6calc -q $ipv6calc_options (rc=$?)"
		exit 1
	fi

	$debug && echo "DEBUG : execution result in entries (array): $ipv6calc -q $ipv6calc_options (${#list_entries_array[@]})"

	if [ ${#list_entries_array[@]} -eq 0 ]; then
		echo "NOTICE: execution returns 0 entries: $ipv6calc -q $ipv6calc_options (skip)"
		continue
	fi

	# convert entries with default value to hash
	for entry in ${list_entries_array[@]}; do
		list_entries[$entry]=0
	done

	$quiet || echo "INFO  : execution result in entries: $ipv6calc -q $ipv6calc_options (${#list_entries[@]})"

	# retrieve 'iplist'
	setname="${prefix}_${countrycode}_${list}"

	unset ipset_entries
	unset ipset_entries_array
	declare -A ipset_entries
	declare -a ipset_entries_array
	ipset_new=false

	# check whether list exits at all
	ipset_info=$(sudo ipset list -t $setname 2>/dev/null)
	rc=$?
	if [ $rc -ne 0 ]; then
		$quiet || echo "NOTICE: ipset is not existing (will be created later): $setname (rc=$?)"
		ipset_new=true
	else
		$quiet || echo "NOTICE: ipset is already existing (will be updated): $setname"

		# retrieve list
		ipset_entries_array=( $(ipset_list_members $setname) )
		rc=$?

		if [ $rc -ne 0 ]; then
			echo "ERROR : execution not successful: ipset_list_members $setname (rc=$?)"
			exit 1
		fi

		$debug && echo "DEBUG : execution result in entries (array): ipset_list_members $setname (${#ipset_entries_array[@]})"

		if [ ${#ipset_entries_array[@]} -eq 0 ]; then
			echo "NOTICE: execution returns 0 entries: ipset_list_members $setname (skip)"
			continue
		fi

		# convert entries to hash with default value
		for entry in ${ipset_entries_array[@]}; do
			ipset_entries[$entry]=0
		done

		$quiet || echo "INFO  : execution result in entries: ipset_list_members $setname (${#ipset_entries[@]})"
	fi

	## Match code
	declare -A statistics
	statistics[Found]=0
	statistics[Delete]=0
	statistics[Add]=0

	for entry in ${!ipset_entries[@]}; do
		if [ -n "${list_entries[$entry]}" ]; then
			# iplist entry exists already in countrycode list, mark with 9
			ipset_entries[$entry]=9
			list_entries[$entry]=9
			statistics[Found]=$[ ${statistics[Found]} + 1 ]
		fi
	done

	for entry in ${!ipset_entries[@]}; do
		if [ "${ipset_entries[$entry]}" -eq 0 ]; then
			# ipset entry not found in countrycode list, mark with 2 -> delete
			ipset_entries[$entry]=2
			statistics[Delete]=$[ ${statistics[Delete]} + 1 ]
		fi
	done

	for entry in ${!list_entries[@]}; do
		$trace && echo "list_entry: $entry"
		if [ "${list_entries[$entry]}" -eq 0 ]; then
			# list entry not found in ipset list, mark with 3 -> add
			list_entries[$entry]=3
			statistics[Add]=$[ ${statistics[Add]} + 1 ]
		fi
	done

	# print statistics
	if ! $quiet; then
		echo -n "INFO  : match statistics for $list:"
		for entry in ${!statistics[@]}; do
			echo -n " $entry=${statistics[$entry]}"
		done
		echo
	fi

	# create command list
	commandlist=$(
		# create set
		if $ipset_new; then
			echo "create $setname hash:net family $family counters"
		fi

		# add missing ones
		for entry in ${!list_entries[@]}; do
			$trace && echo "list_entry: $entry"
			if [ "${list_entries[$entry]}" -eq 3 ]; then
				echo "add $setname $entry"
			fi
		done

		# delete no longer in list existing ones
		for entry in ${!ipset_entries[@]}; do
			if [ "${ipset_entries[$entry]}" -eq 2 ]; then
				echo "del $setname $entry"
			fi
		done
	)

	if [ -z "$commandlist" ]; then
		$quiet || echo "NOTICE: nothing todo for $list and $setname"
		summary[$list]="nothing-todo"
		continue
	fi


	if $dryrun; then
		if $debug; then
			echo "NOTICE: dry-run selected <BEGIN>"
			echo "$commandlist"
			echo "NOTICE: dry-run selected <END> (use -n for no-dryrun execution)"
		fi
		summary[$list]="dryrun"
		require_no_dryrun=true
	else
		# create temporary file
		commandfile=$(mktemp /tmp/ipset-$list-$setname.XXXXX)
		if [ -z "$commandfile" ]; then
			echo "ERROR : can't create temporary command file"
			exit 1
		fi
		echo "$commandlist" >$commandfile
		sudo ipset - <$commandfile >/dev/null
		rc=$?
		if [ $rc -ne 0 ]; then
			echo "ERROR : can't execute commands from commandfile via sudo: $commandfile ($setname)"
			summary[$list]="create/update-ERROR"
			result=1
			continue
		fi

		summary[$list]="create/update-successful"

		if $debug; then
			echo " NOTICE : keep command file: $commandfile"
		else
			[ -e "$commandfile" ] && rm -f "$commandfile"
		fi
	fi
done

if ! $quiet; then
	echo -n "NOTICE: summary:"
	for entry in ${!summary[@]}; do
		echo -n " $entry=${summary[$entry]}"
	done
	if $require_no_dryrun; then
		echo -n " (pending actions require no-dryrun [-n])"
	fi
	echo
fi

exit $result
