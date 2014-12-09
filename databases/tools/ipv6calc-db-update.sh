#!/bin/bash
#
# Simple shell script to update and generate ipv6calc (external) database files
#
# Project    : ipv6calc/external database
# File       : ipv6calc-db-update.sh
# Version    : $Id: ipv6calc-db-update.sh,v 1.1 2014/12/09 21:03:51 ds6peter Exp $
# Copyright  : 2014-2014 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL version 2

# Definitions:
IPV6CALC_DAT_DIR="${IPV6CALC_DAT_DIR:-/usr/share/ipv6calc/db/}"
IPV6CALC_TOOLS_DIR="${IPV6CALC_TOOLS_DIR:-/usr/share/ipv6calc/tools/}"

if [ ! -d "$IPV6CALC_DAT_DIR" ]; then
	echo "ERROR : missing directory: $IPV6CALC_DAT_DIR (IPV6CALC_DAT_DIR)"
	exit 1
fi

if [ ! -w "$IPV6CALC_DAT_DIR" ]; then
	echo "ERROR : missing write permissions on directory: $IPV6CALC_DAT_DIR (IPV6CALC_DAT_DIR)"
	exit 1
fi

## Download
$IPV6CALC_TOOLS_DIR/update-registries.sh -D "$IPV6CALC_DAT_DIR"
if [ $? -ne 0 ]; then
	echo "ERROR : update of registry files failed"
	exit 1
fi

## Generate
$IPV6CALC_TOOLS_DIR/ipv4-create-registry-list.pl -B -S "$IPV6CALC_DAT_DIR" -D "$IPV6CALC_DAT_DIR"
if [ $? -ne 0 ]; then
	echo "ERROR : problem during generation of IPv4 related databases"
	exit 1
fi

$IPV6CALC_TOOLS_DIR/ipv6-create-registry-list.pl -B -S "$IPV6CALC_DAT_DIR" -D "$IPV6CALC_DAT_DIR"
if [ $? -ne 0 ]; then
	echo "ERROR : problem during generation of IPv6 related databases"
	exit 1
fi
