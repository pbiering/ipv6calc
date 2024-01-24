#!/bin/bash
#
# Support shell script for conditional restart after database file update
#           
# Project    : ipv6calc/
# File       : ipv6calc-db-update-support.sh
# Copyright  : 2024-2024 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL version 2

ipv6calc_db_update_support_help() {
	cat <<END
	-R		restart services (having open but replaced database files)
			    requires "sudo" in case of non-root:
				/usr/bin/systemctl restart *
				/usr/bin/systemctl is-active *
				/usr/bin/lsof *
	-r		test-run for services restart
END
}

ipv6calc_db_update_support_reloadrestart() {
	dat_dir="$1"
	testrun="$2"

	if [ -z "$dat_dir" ]; then
		echo "ERROR : reload/restart only supported if data directory is given"
		return 1
	fi

	if [ ! -d "$dat_dir" ]; then
		echo "ERROR : reload/restart only supported if data directory is existing: $dat_dir"
		return 1
	fi

	if [ $UID -ne 0 ]; then
		sudo="/usr/bin/sudo"
	else
		sudo=""
	fi

	declare -A pidlist
	declare -A servicelist
	declare -A filelist
	# found open but deleted files of FD=DEL TYPE=REG (mapped)
	for line in $($sudo /usr/bin/lsof -l -n -P -M -w -b -L -d '^0,^1,^2' -E | awk "\$1 != \"PID\" && \$4 == \"DEL\" && \$5 == \"REG\" { if (substr(\$8, 0, length(\"$dat_dir\")) == \"$dat_dir\" ) print \$2 \"|\" \$8 }" | sort -u); do
		pid="${line/|*}"
		file="${line/*|}"
		pidlist[$pid]=1
		if [ -n "${filelist[$pid]}" ]; then
			filelist[$pid]="${filelist[$pid]}|$file"
		else
			filelist[$pid]="$file"
		fi
	done

	# found open but deleted files of TYPE=REG (deleted)
	for line in $($sudo /usr/bin/lsof -l -n -P +L1 -E | awk "\$1 != \"PID\" && \$5 == \"REG\" { if (substr(\$10, 0, length(\"$dat_dir\")) == \"$dat_dir\" ) print \$2 \"|\" \$10 }" | sort -u); do
		pid="${line/|*}"
		file="${line/*|}"
		pidlist[${line/|*}]=1
		if [ -n "${filelist[$pid]}" ]; then
			filelist[$pid]="${filelist[$pid]}|$file"
		else
			filelist[$pid]="$file"
		fi
	done

	if [ ! -x /usr/bin/systemctl ]; then
		## for now only systemd is supported
		echo "NOTICE: missing binary: /usr/bin/systemctl (unsupported system)"
		return 0
	fi

	## turn pidlist into servicelist
	for pid in ${!pidlist[*]}; do
		service=$(/usr/bin/systemctl status $pid | awk '{ print $2; exit }')
		if [ -n "$service" ]; then
			servicelist[$service]=1
			if [ -n "${filelist[$service]}" ]; then
				filelist[$service]="${filelist[$service]}|${filelist[$pid]}"
			else
				filelist[$service]="${filelist[$pid]}"
			fi
		fi
	done

	if [ ${#servicelist[*]} -eq 0 ]; then
		echo "INFO  : no service found having open but replaced database files in directory: $dat_dir"
		return 0
	fi

	for service in ${!servicelist[*]}; do
		echo "NOTICE: check service having open but replaced database files: $service"
		echo "${filelist[$service]}" | sed  "s/|/\n/g" | sort | uniq | while read line; do echo "INFO  : $service: $line"; done
		if ! /usr/bin/systemctl -q is-active $service; then
			echo "WARN  : service is not active, but still open replaced database files: $service"
			continue
		fi

		if [ "$testrun" = "1" ]; then
			echo "NOTICE: (TEST) would execute: $sudo /usr/bin/systemctl restart $service"
		else
			echo "NOTICE: execute: $sudo /usr/bin/systemctl restart $service"
			$sudo /usr/bin/systemctl restart $service
			if [ $? -eq 0 ]; then
				echo "INFO: execute: $sudo /usr/bin/systemctl restart $service (SUCCESSFUL)"
			else
				echo "WARN  : execute: $sudo /usr/bin/systemctl restart $service (PROBLEM)"
			fi
		fi
	done
}

have_ipv6calc_db_update_support=1
