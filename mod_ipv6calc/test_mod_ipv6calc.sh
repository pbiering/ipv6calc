#!/bin/sh
#
# Project    : ipv6calc
# File       : test_mod_ipv6calc.sh
# Version    : $Id: test_mod_ipv6calc.sh,v 1.2 2015/07/08 20:32:06 ds6peter Exp $
# Copyright  : 2015-2015 by Peter Bieringer <pb (at) bieringer.de>
#
# Test patterns for ipv6calc conversions

verbose=0


create_apache_root_and_start() {
	echo "INFO  : create temporary base directory"
	dir_base=$(mktemp -d /tmp/mod_ipv6calc.XXXXX)
	if [ -z "$dir_base" ]; then
		echo "ERROR : can't create temporary directory"
		return 1
	fi
	echo "INFO  : temporary base directory created: $dir_base"

	for dir in $dir_base/conf $dir_base/var $dir_base/modules $dir_base/logs $dir_base/conf.d $dir_base/conf.modules.d; do
		echo "INFO  : create directory: $dir"
		mkdir $dir
		if [ $? -ne 0 ]; then
			echo "ERROR : can't create directory: $dir"
			return 1
		fi
		echo "INFO  : directory created: $dir"
	done

	for file in /etc/httpd/conf/httpd.conf /etc/httpd/conf/magic; do
		echo "INFO  : copy file: $file -> $dir_base/conf"
		cp -p $file $dir_base/conf/
		if [ $? -ne 0 ]; then
			echo "ERROR : can't copy file: $file"
			return 1
		fi
		echo "INFO  : file copied: $file -> $dir_base/conf"
	done

	echo "INFO  : define listen port 8080 in $dir_base/conf/httpd.conf"
	perl -pi -e 's/^Listen/Listen 8080/g' $dir_base/conf/httpd.conf
	if [ $? -ne 0 ]; then
		echo "ERROR : can't define listen port: $dir_base/conf/httpd.conf"
		return 1
	fi

	echo "INFO  : change log level to debug $dir_base/conf/httpd.conf"
	perl -pi -e 's/^LogLevel/LogLevel debug/g' $dir_base/conf/httpd.conf
	if [ $? -ne 0 ]; then
		echo "ERROR : can't define LogLevel: $dir_base/conf/httpd.conf"
		return 1
	fi

	echo "INFO  : specify pid file in $dir_base/conf/httpd.conf"
	echo "PidFile $dir_base/var/httpd.pid" >>$dir_base/conf/httpd.conf
	if [ $? -ne 0 ]; then
		echo "ERROR : can't specify pid file: $dir_base/conf/httpd.conf"
		return 1
	fi

	perl -pi -e 's/^ServerRoot.*$//g' $dir_base/conf/httpd.conf

	for file in 00-base.conf 00-mpm.conf; do
		cp /etc/httpd/conf.modules.d/$file $dir_base/conf.modules.d/
	done

	# disable auth modules
	perl -pi -e 's/^LoadModule authn.*$//g' $dir_base/conf.modules.d/00-base.conf
	perl -pi -e 's/^LoadModule auth_digest_module.*$//g' $dir_base/conf.modules.d/00-base.conf

	cp /etc/httpd/modules/* $dir_base/modules/

	# copy mod_ipv6calc
	cp .libs/mod_ipv6calc.so $dir_base/modules/
	cp ipv6calc.conf $dir_base/conf.d/

	## adjust ipv6calc.conf settings	
	# load module
	perl -pi -e 's/#(LoadModule.*)/$1/g' $dir_base/conf.d/ipv6calc.conf

	# enable module
	perl -pi -e 's/#(ipv6calcEnable).*/$1 on/g' $dir_base/conf.d/ipv6calc.conf

	# enable anonymized access logging
	perl -pi -e 's/#(CustomLog.*)/$1/g' $dir_base/conf.d/ipv6calc.conf

	if [ "$debug_module" = "1" ]; then
		# enable module debugging
		perl -pi -e 's/#(ipv6calcDebuglevel.*-1)/$1/g' $dir_base/conf.d/ipv6calc.conf
	fi

	if [ "$debug_library" = "1" ]; then
		# enable library debugging
		perl -pi -e 's/#(ipv6calcOption).*/$1 debug -1/g' $dir_base/conf.d/ipv6calc.conf
	fi

	echo "INFO  : start httpd with ServerRoot $dir_base"
	httpd -X -e info -d $dir_base &
	if [ $? -eq 0 ]; then
		echo "INFO  : httpd started in background"
	fi

	limit=10
	i=0
	while [ ! -f $dir_base/var/httpd.pid ]; do
		i=$[ $i + 1 ]
		if [ $i -gt 10 ]; then
			break
		fi
		sleep 1
	done

	pid="$(cat "$dir_base/var/httpd.pid")"

	if [ -z "$pid" ]; then
		echo "ERROR : httpd PID file empty: $dir_base/var/httpd.pid"
		return 1
	fi

	if ! ps -p $pid >/dev/null 2>&1; then
		echo "ERROR : httpd started with PID but no longer running: $pid"
		return 1
	fi

	echo "INFO  : httpd started with PID: $pid"

	if [ -f $dir_base/logs/error_log ]; then
		grep ipv6calc $dir_base/logs/error_log
		lines_error_log=$(cat "$dir_base/logs/error_log" | wc -l)
	fi

	echo "NOTICE: base directory is    : $dir_base"
	echo "NOTICE: error log            : $dir_base/logs/error_log"
	echo "NOTICE: access log           : $dir_base/logs/access_log"
	echo "NOTICE: anonymized access log: $dir_base/logs/access_anon_log"
}

stop_apache() {
	if [ -z "$pid" -a -n "$dir_base" -a -f "$dir_base/var/httpd.pid" ]; then
		pid="$(cat "$dir_base/var/httpd.pid")"
	fi

	if [ -n "$pid" ]; then
		echo "NOTICE: kill started httpd with PID: $pid"
		kill $pid
	else
		echo "WARN  : no httpd PID given, use killall"
		killall httpd
	fi
}


exec_request() {
	dst="$1"

	echo "NOTICE: test: $1"
	curl -s "http://$1:8080/" >/dev/null
	if [ $? -ne 0 ]; then
		echo "ERROR : curl request to $1:8080 failed"
		return 1
	fi

	echo "INFO  : access log entry"
	tail -1 $dir_base/logs/access_log

	if [ -f "$dir_base/logs/access_anon_log" ]; then
		echo "INFO  : anonymized access log entry"
		tail -1 $dir_base/logs/access_anon_log
	else
		echo "ERROR : anonymized access log missing"
		return 1
	fi

	echo "INFO  : error log entry"
	tail -n +$[ $lines_error_log + 1 ] "$dir_base/logs/error_log"
}


run_test_requests() {
	echo "NOTICE: run test requests"

	if [ -n "$address" ]; then
		if echo "$address" | grep -q ":"; then
			# IPv6
			exec_request "[$address]"
		else
			# IPv4
			exec_request "$address"
		fi
	else
		for ipv4 in $(ip -o addr show |grep -w inet | grep -w global | grep -vw deprecated | awk '{ print $4 }' | awk -F/ '{ print $1 }'); do
			exec_request "$ipv4" || return 1
		done

		# retrieve local IPv6 address
		for ipv6 in $(ip -o addr show |grep -w inet6 | grep -w global | grep -vw deprecated | awk '{ print $4 }' | awk -F/ '{ print $1 }'); do
			exec_request "[$ipv6]" || return 1
		done
	fi

	sleep 2

	return 0
}


#### Options
while getopts "a:SKWb:mlh\?" opt; do
	case $opt in
	    b)
		if [ -d "$OPTARG" ]; then
			dir_base=$OPTARG
		else
			echo "ERROR : given base directory doesn't exist: $dir_base"
		fi
		;;
	    m)
		debug_module=1
		;;
	    l)
		debug_library=1
		;;
	    S)
		create_apache_root_and_start || exit 1
		;;
	    K)
		stop_apache || exit 1
		;;
	    a)
		address="$OPTARG"
		;;
	    W)
		create_apache_root_and_start || exit 1
		run_test_requests || exit 1
		stop_apache || exit 1
		;;
	    *)
		echo "$0 [-m] [-l] -S			start"
		echo "      -m	enable debug module"
		echo "      -l	enable debug library"
		echo "$0 -K -b <base directory>		stop"
		echo "$0 [-m] [-l] [-a <address>] -W	run workflow"
		exit 1
		;;
	esac
done

