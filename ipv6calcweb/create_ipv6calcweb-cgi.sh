#!/usr/bin/env bash
#
# Project    : ipv6calc/ipv6calcweb
# File       : create_ipv6calcweb.sh
# Version    : $Id$
# Copyright  : 2013-2021 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  create ipv6calcweb.cgi from ipv6calcweb.cgi.in
#

# check
if [ ! -f ../config.h ]; then
	echo "ERROR : missing header file: ../config.h"
	exit 1
fi

# get placeholders
v="`cat ../config.h | grep -w PACKAGE_VERSION | awk '{ print $3 }' | sed 's/"//g'`"
c="`cat ../config.h | grep -w COPYRIGHT_YEAR | awk '{ print $3 }' | sed 's/"//g'`"

if [ -z "$v" ]; then
	echo "ERROR : can't retrieve version from config.h"
	exit 1
fi

if [ -z "$c" ]; then
	echo "ERROR : can't retrieve copyright year from config.h"
	exit 1
fi

cp ipv6calcweb.cgi.in ipv6calcweb.cgi || exit 1

# replace placeholders
perl -pi -e "s/\@PACKAGE_VERSION\@/$v/" ipv6calcweb.cgi || exit 1
perl -pi -e "s/\@COPYRIGHT_YEAR\@/$c/" ipv6calcweb.cgi || exit 1

case "$OSTYPE" in
    freebsd*)
	# FreeBSD has somehow issue with -T, so remove it
	perl -pi -e "s/perl -w -T/perl -w/" ipv6calcweb.cgi || exit 1
	;;
esac

if [ ! -x ipv6calcweb.cgi ]; then
	chmod u+x ipv6calcweb.cgi
fi

if [ -f ipv6calcweb.cgi ]; then
	touch ipv6calcweb.cgi -r ipv6calcweb.cgi.in
fi

echo "INFO  : successfully created: ipv6calcweb.cgi"
