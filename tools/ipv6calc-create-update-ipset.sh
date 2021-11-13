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
# poweruser     ALL=NOPASSWD:/usr/bin/firewall-cmd
#
# Usage in combination with firewalld
#  Note: ipset counters are not supported by ipset generation through firewalld
#  See also: https://firewalld.org/documentation/man-pages/firewalld.ipset.html
#   firewall-cmd --permanent --zone=block --add-source=ipset:Net_VN_IPv4
#   firewall-cmd --permanent --zone=block --add-source=ipset:Net_VN_IPv6
#   firewall-cmd --permanent --zone=block --add-source=ipset:Net_VN_IPv6to4
#   firewall-cmd --permanent --zone=public --add-rich-rule="rule family='ipv4' source ipset='Net_VN_IPv4'    port port='8080' protocol='tcp' drop"
#   firewall-cmd --permanent --zone=public --add-rich-rule="rule family='ipv6' source ipset='Net_VN_IPv6'    port port='8080' protocol='tcp' drop"
#   firewall-cmd --permanent --zone=public --add-rich-rule="rule family='ipv6' source ipset='Net_VN_IPv6to4' port port='8080' protocol='tcp' drop"
#
# Usage example in combination with iptables/ip6tables (to become reboot-safe take use of 'ipset-service')
#   iptables  -I INPUT -m set --match-set Net_VN_IPv4    src -j DROP
#   ip6tables -I INPUT -m set --match-set Net_VN_IPv6    src -j DROP
#   ip6tables -I INPUT -m set --match-set Net_VN_IPv6to4 src -j DROP
#   iptables  -I INPUT -m set --match-set Net_VN_IPv4    src -p tcp --dport 8080 -j DROP
#   ip6tables -I INPUT -m set --match-set Net_VN_IPv6    src -p tcp --dport 8080 -j DROP
#   ip6tables -I INPUT -m set --match-set Net_VN_IPv6to4 src -p tcp --dport 8080 -j DROP
#
# 20210929/bie: initial version
# 20211001/bie: add support for firewalld
# 20211003/bie: further improvements


prefix="Net"

help() {
	cat <<END
Usage: $(basename "$0") -C <CountryCode> [-P <SETNAME-Prefix>] [-L <list>] [-F [-p]] [-n] [-d] [-q] [-t]
                        -h|?
	-C <CountryCode>	Country Code (CC) for database filter
	-F			control 'ipset' via 'firewalld'
	-p			control 'firewalld' permanent configuration (reboot-safe)
	-n			no dry-run

	-P <SETNAME-Prefix>	prefix for 'ipset' SETNAME (optional, default: $prefix)
	-L <list>>		selected list (IPv4|IPv6|IPv6to4) (optional, default: ALL)

	-q			more quiet (default if called by cron)
	-d			debug
	-t			trace

	-h|?			this online help

honors environment
	IPV6CALC_BIN		'ipv6calc' binary

'ipset' SETNAME will be named: ${prefix}_CC_(IPv4|IPv6|IPv6to4)
END
}

## defines
declare -A summary
summary_important=false
summary_hint="ipset"

## default
dryrun=true
debug=false
trace=false
firewalld=false
firewalld_permanent=""

if [ -t 0 ]; then
	quiet=false
else
	quiet=true
fi

## parse options
while getopts "\?hdnqtFpC:P:L:" opt; do
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
	    F)
		firewalld=true
		summary_hint="firewalld"
		;;
	    p)
		if ! $firewalld; then
			echo "NOTICE: -p selected but -F not given"
			exit 1
		else
			firewalld_permanent=1
			summary_hint="firewalld/permanent"
		fi
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

# check ipset
ipset_check() {
	local setname="$1"

	if $firewalld; then
		$debug && echo "DEBUG : called: ipset info $setname (firewalld)" >&2
		sudo firewall-cmd ${firewalld_permanent:+--permanent }--info-ipset=$setname >/dev/null 2>&1
		rc=$?
	else
		$debug && echo "DEBUG : called: ipset info $setname" >&2
		sudo ipset list -t $setname >/dev/null 2>&1
		rc=$?
	fi

	$debug && echo "DEBUG : ipset info $setname (rc=$rc)" >&2
	return $rc
}

# list entries of an ipset
ipset_list_entries() {
	local setname="$1"

	if $firewalld; then
		$debug && echo "DEBUG : called: ipset_list_entries $setname (firewalld)" >&2
		sudo firewall-cmd ${firewalld_permanent:+--permanent }--ipset=$setname --get-entries
	else
		$debug && echo "DEBUG : called: ipset_list_entries $setname" >&2

		# note: ipset will always print header, this is currently not deselectable
		sudo ipset list $setname -o xml 2>/dev/null | while read line; do
			$trace && echo "DEBUG : ipset_list_entries: $line (TEST)" >&2
			if [[ $line =~ \<member\>\<elem\>([0-9a-f:./]+)\<\/elem\> ]]; then
				$trace && echo "DEBUG : ipset_list_entries: $line (MATCH)" >&2
				echo "${BASH_REMATCH[1]}"
			fi
		done
	fi
}

# update entries of an ipset (native)
# rc: 0=ok 1=nothingtodo 4=dryrun 3=error 2=fatal
ipset_update_entries() {
	# create command list
	commandlist=$(
		# create set
		if $ipset_new; then
			echo "create $setname hash:net family $family counters"
		fi

		# add missing ones
		for entry in ${!list_entries[@]}; do
			if [ "${list_entries[$entry]}" -eq 3 ]; then
				$trace && echo "list_entry (add): $entry" >&2
				echo "add $setname $entry"
			fi
		done

		# delete no longer in list existing ones
		for entry in ${!ipset_entries[@]}; do
			if [ "${ipset_entries[$entry]}" -eq 2 ]; then
				$trace && echo "list_entry (del): $entry" >&2
				echo "del $setname $entry"
			fi
		done
	)

	if [ -z "$commandlist" ]; then
		$quiet || echo "NOTICE: nothing todo for $list and $setname"
		return 1
	fi


	if $dryrun; then
		if $trace; then
			echo "NOTICE: dry-run selected <BEGIN>"
			echo "$commandlist"
			echo "NOTICE: dry-run selected <END> (use -n for no-dryrun execution)"
		fi
		return 4
	else
		# create temporary file
		commandfile=$(mktemp /tmp/ipset-$list-$setname.XXXXX)
		if [ -z "$commandfile" ]; then
			echo "ERROR : can't create temporary command file"
			return 2
		fi
		echo "$commandlist" >$commandfile
		sudo ipset - <$commandfile >/dev/null
		rc=$?
		if [ $rc -ne 0 ]; then
			echo "ERROR : can't execute commands from commandfile via sudo: $commandfile ($setname)"
			return 3
		fi

		if $debug; then
			echo " NOTICE : keep command file: $commandfile"
		else
			[ -e "$commandfile" ] && rm -f "$commandfile"
		fi
	fi
	return 0
}

# update entries of an ipset (firewalld)
# rc: 0=ok 1=nothingtodo 4=dryrun 3=error 2=fatal
ipset_update_entries_firewalld() {
	local add_entries_file=$(mktemp /tmp/ipset-firewalld-add-$list-$setname.XXXXX)
	if [ -z "$add_entries_file" ]; then
		echo "ERROR : can't create temporary file containing entries to add"
		return 2
	fi

	# add missing ones
	for entry in ${!list_entries[@]}; do
		if [ "${list_entries[$entry]}" -eq 3 ]; then
			$trace && echo "list_entry (add): $entry" >&2
			echo "$entry" >>$add_entries_file
		fi
	done

	local del_entries_file=$(mktemp /tmp/ipset-firewalld-remove-$list-$setname.XXXXX)
	if [ -z "$del_entries_file" ]; then
		echo "ERROR : can't create temporary file containing entries to remove"
		return 2
	fi

	# delete no longer in list existing ones
	for entry in ${!ipset_entries[@]}; do
		if [ "${ipset_entries[$entry]}" -eq 2 ]; then
			$trace && echo "list_entry (del): $entry" >&2
			echo "$entry" >$del_entries_file
		fi
	done

	if [ ! -s $add_entries_file -a ! -s $del_entries_file ]; then
		$quiet || echo "NOTICE: nothing todo for $list and $setname"
		return 1
	fi

	# create command list
	commandlist=$(
		if $ipset_new; then
			# new ipset is always permanent	
			echo "firewall-cmd --permanent --new-ipset=$setname --type=hash:net --option=family=$family"
			echo "firewall-cmd --reload"
		fi

		if [ -s $add_entries_file ]; then
			echo "firewall-cmd ${firewalld_permanent:+--permanent }--ipset=$setname --add-entries-from-file=$add_entries_file"
		fi

		if [ -s $del_entries_file ]; then
			echo "firewall-cmd ${firewalld_permanent:+--permanent }--ipset=$setname --remove-entries-from-file=$del_entries_file"
		fi

		if [ -n "$firewalld_permanent" ]; then
			echo "firewall-cmd --reload"
		fi
	)


	if $dryrun; then
		if $debug; then
			echo "NOTICE: dry-run selected <BEGIN>"
			echo "$commandlist"
			echo "NOTICE: dry-run selected <END> (use -n for no-dryrun execution)"
		fi
		return 4
	else
		echo "$commandlist" | while read line; do
			if $debug; then
				echo "DEBUG : execute via sudo: $line"
				sudo $line
				rc=$?
			else
				sudo $line >/dev/null
				rc=$?
			fi

			if $debug; then
				echo "DEBUG : executed via sudo: $line (rc=$rc)"
			fi

			[ $rc -ne 0 ] && exit 3
		done

		if [ $rc -eq 3 ]; then
			echo "ERROR : can't execute commands from commandlist via sudo: ($setname)"
			return 3
		fi

		if $debug; then
			echo "NOTICE: keep add/remove entries files: $add_entries_file $del_entries_file"
		else
			[ -e "$add_entries_file" ] && rm -f "$add_entries_file"
			[ -e "$del_entries_file" ] && rm -f "$del_entries_file"
		fi
	fi
	return 0
}


## main work
require_no_dryrun=false
result=0

for list in ${!ipv6calc_support[@]}; do
	if [ -n "$listselected" -a "$list" != "$listselected" ]; then
		$quiet || echo "INFO  : skip not selected list for country code: $list ($countrycode)"
		summary[$list]="SKIPPED"
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
	ipset_check $setname
	rc=$?

	if [ $rc -ne 0 ]; then
		$quiet || echo "NOTICE: ipset is not existing (will be created later): $setname (rc=$?)"
		ipset_new=true
	else
		$quiet || echo "NOTICE: ipset is already existing (will be updated): $setname"

		# retrieve list
		ipset_entries_array=( $(ipset_list_entries $setname) )
		rc=$?

		if [ $rc -ne 0 ]; then
			echo "ERROR : execution not successful: ipset_list_entries $setname (rc=$?)"
			exit 1
		fi

		$debug && echo "DEBUG : execution result in entries (array): ipset_list_entries $setname (${#ipset_entries_array[@]})"

		# convert entries to hash with default value
		for entry in ${ipset_entries_array[@]}; do
			ipset_entries[$entry]=0
		done

		$quiet || echo "INFO  : execution result in entries: ipset_list_entries $setname (${#ipset_entries[@]})"
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

	# create/update
	if $firewalld; then
		ipset_update_entries_firewalld
		rc=$?
	else
		ipset_update_entries
		rc=$?
	fi

	if [ $rc -eq 2 ]; then
		summary[$list]="FATAL"
		summary_important=true
		[ $result -eq 0 ] && result=1
		break
	elif [ $rc -eq 1 ]; then
		summary[$list]="NOOP"
	elif [ $rc -eq 0 ]; then
		summary[$list]="SUCCESS(add=${statistics[Add]}/del=${statistics[Delete]})"
		summary_important=true
	elif [ $rc -eq 3 ]; then
		summary[$list]="ERROR"
		summary_important=true
		[ $result -eq 0 ] && result=1
	elif [ $rc -eq 4 ]; then
		summary[$list]="DRYRUN"
		require_no_dryrun=true
		summary_important=true
	fi
done

if ! $quiet || $summary_important; then
	echo -n "NOTICE: summary for $countrycode ($summary_hint):"
	for entry in ${!summary[@]}; do
		echo -n " $entry=${summary[$entry]}"
	done
	if $require_no_dryrun; then
		echo -n " (pending actions require no-dryrun [-n])"
	fi
	if [ ! -n "$firewalld_permanent" ]; then
		echo -n " (firewalld permanent change requires -p)"
	fi
	echo
fi

exit $result
