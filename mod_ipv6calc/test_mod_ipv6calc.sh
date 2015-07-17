#!/bin/sh
#
# Project    : ipv6calc
# File       : test_mod_ipv6calc.sh
# Version    : $Id: test_mod_ipv6calc.sh,v 1.7 2015/07/17 05:14:10 ds6peter Exp $
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
	perl -pi -e 's/^LogLevel .*/LogLevel debug/g' $dir_base/conf/httpd.conf
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
		perl -pi -e 's/#(ipv6calcOption\s+debug).*/$1 -1/g' $dir_base/conf.d/ipv6calc.conf
	fi

	## disable databases by option
	[ "$disable_geoip" = "1" ]       && perl -pi -e 's/#(ipv6calcOption\s+db-geoip-disable\s+yes)$/$1/g' $dir_base/conf.d/ipv6calc.conf
	[ "$disable_ip2location" = "1" ] && perl -pi -e 's/#(ipv6calcOption\s+db-ip2location-disable\s+yes)$/$1/g' $dir_base/conf.d/ipv6calc.conf
	[ "$disable_dbip" = "1" ]        && perl -pi -e 's/#(ipv6calcOption\s+db-dbip-disable\s+yes)$/$1/g' $dir_base/conf.d/ipv6calc.conf
	[ "$disable_external" = "1" ]    && perl -pi -e 's/#(ipv6calcOption\s+db-external-disable\s+yes)$/$1/g' $dir_base/conf.d/ipv6calc.conf


	echo "INFO  : start httpd with ServerRoot $dir_base"
	/usr/sbin/httpd -X -e info -d $dir_base &
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

	
	if [ "$debug_lsof" = "1" ]; then
		echo "INFO  : list of open files of pid: $pid"
		lsof -p $pid
	fi

	if [ "$debug_config" = "1" ]; then
		echo "INFO  : list effective module config"
		grep -v "^\s*#" $dir_base/conf.d/ipv6calc.conf | grep -v "^\s*$"
	fi

	echo "NOTICE: base directory is     : $dir_base"
	echo "NOTICE: ipv6calc module config: $dir_base/conf.d/ipv6calc.conf"
	echo "NOTICE: error log             : $dir_base/logs/error_log"
	echo "NOTICE: access log            : $dir_base/logs/access_log"
	echo "NOTICE: anonymized access log : $dir_base/logs/access_anon_log"
}

stop_apache() {
	if [ -z "$pid" -a -n "$dir_base" -a -f "$dir_base/var/httpd.pid" ]; then
		pid="$(cat "$dir_base/var/httpd.pid")"
	fi

	if [ -n "$pid" ]; then
		echo "NOTICE: kill started httpd with PID: $pid"
		kill $pid
	else
		if [ ! -x /usr/bin/killall ]; then
			echo "ERROR : no httpd PID given, but no 'killall' available (provided by RPM: psmisc)"
			echo "ERROR : you have to kill process manually"
		else
			echo "WARN  : no httpd PID given, use killall"
			/usr/bin/killall httpd
		fi
		
	fi
}


exec_request() {
	dst="$1"

	echo "NOTICE: test: $1"
	# curl-7.29.0-19.el7.x86_64 is broken, -g required
	curl -g -s "http://$1:8080/" >/dev/null
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
		for ipv4 in $(/sbin/ip -o addr show |grep -w inet | grep -w global | grep -vw deprecated | awk '{ print $4 }' | awk -F/ '{ print $1 }'); do
			exec_request "$ipv4" || return 1
		done

		# retrieve local IPv6 address
		for ipv6 in $(/sbin/ip -o addr show |grep -w inet6 | grep -w global | grep -vw deprecated | awk '{ print $4 }' | awk -F/ '{ print $1 }'); do
			exec_request "[$ipv6]" || return 1
		done
	fi

	sleep 2

	return 0
}

#### Help
help() {
	cat <<END
$(basname "$0") [<options>] [-S|-K|-W]
	-S	start
	-K	stop (kill)
	-W	run workflow

	-m	enable debug module
	-l	enable debug library
	-f	list open files after start
	-c	show effective module config options
	-g	disable GeoIP
	-i	disable IP2Location
	-d	disable DBIP.com
	-e	disable external databases

	-b <base directory

	-a <address>	disable autoretrievement of local IP, use given one instead
END
}

#### Options
while getopts "ca:fSKWb:mlgideh\?" opt; do
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
	    f)
		debug_lsof=1
		;;
	    c)
		debug_config=1
		;;
	    g)
		disable_geoip=1
		;;
	    i)
		disable_ip2location=1
		;;
	    d)
		disable_dbip=1
		;;
	    e)
		disable_external=1
		;;
	    S)
		action="start"
		;;
	    K)
		action="kill"
		;;
	    a)
		address="$OPTARG"
		;;
	    W)
		action="workflow"
		;;
	    h|\?)
		help
		exit 1
		;;
	    *)
		echo "ERROR : unrecognized option: $opt"
		;;
	esac
done

case $action in
    workflow)
	create_apache_root_and_start || exit 1
	run_test_requests || exit 1
	stop_apache || exit 1
	;;
    kill)
	stop_apache || exit 1
	;;
    start)
	create_apache_root_and_start || exit 1
	;;
    *)
	help
	exit 1
	;;
esac

