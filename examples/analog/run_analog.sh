#!/bin/sh
#
# Simple shell script to create different types of statistics
#
# Example config file for analog creating some statistics
#
# Project    : ipv6calc/ipv6logconv
# File       : run_analog.sh
# Version    : $Id: run_analog.sh,v 1.5 2002/04/05 18:54:56 peter Exp $
# Copyright  : none
# Procuded   : 2002 by Peter Bieringer <pb (at) bieringer.de>

umask 007

LOGFILE="../access_log"

BIN_IPV6LOGCON="../../ipv6logconv/ipv6logconv"

CONFIG_ANALOG="analog-dist.cfg"
CONFIG_ANALOG_COMBINED="analog-dist-combined.cfg"
DIR_BASE="."

if [ ! -d "$DIR_BASE" ]; then
	echo "Missing base directory: $DIR_BASE"
	exit 2
fi

if [ ! -r "$CONFIG_ANALOG" ]; then
	echo "Missing or unreadable analog config file: $CONFIG_ANALOG"
	exit 2
fi

#set -x

type="$1"

case "$type" in
    "dedicated")
	## dedicated files version
	# Create address type statistics
	echo "addrtype..."
	cat $LOGFILE | $BIN_IPV6LOGCON --out addrtype | analog +g$CONFIG_ANALOG +C'HOSTNAME Address_type_distribution' +O$DIR_BASE/distribution-addrtype/analog.html 

	echo "ipv6addrtype..."
	# Create IPv6 address type statistics
	cat $LOGFILE | $BIN_IPV6LOGCON --out ipv6addrtype | analog +g$CONFIG_ANALOG +C'HOSTNAME IPv6_address_type_distribution' +O$DIR_BASE/distribution-ipv6addrtype/analog.html 
	# Create OUI statistics
	echo "ouitype..."
	cat $LOGFILE | $BIN_IPV6LOGCON --out ouitype | analog +g$CONFIG_ANALOG +C'HOSTNAME OUI_type_distribution' +O$DIR_BASE/distribution-ouitype/analog.html 
	;;

    "combined")
	echo "Create statistics running 'analog'"
	cat $LOGFILE | $BIN_IPV6LOGCON --out any | analog +g$CONFIG_ANALOG_COMBINED +C'HOSTNAME combined' +O$DIR_BASE/distribution-combined/analog.html 

	;;
    *)
	echo "Usage: $0 dedicated|combined"
esac
