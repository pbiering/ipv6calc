#!/bin/sh
#
# Simple shell script to create different types of statistics
#
# Example config file for analog creating some statistics
#
# Project    : ipv6calc/ipv6logconv
# File       : run_analog.sh
# Version    : $Id: run_analog.sh,v 1.2 2002/03/16 22:57:24 peter Exp $
# Copyright  : none
# Procuded   : 2002 by Peter Bieringer <pb (at) bieringer.de>

umask 077

LOGFILE="../access_log"

BIN_IPV6LOGCON="../../ipv6logconv"

FILE_TMP="/tmp/access_log.combined"

#set -x

type="$1"

case "$type" in
    "dedicated")
	## dedicated files version
	# Create address type statistics
	cat $LOGFILE | $BIN_IPV6LOGCON --out addrtype | analog +ganalog-dist.cfg +C'HOSTNAME Address_type_distribution' +O./distribution-addrtype/analog.html 

	# Create IPv6 address type statistics
	cat $LOGFILE | $BIN_IPV6LOGCON --out ipv6addrtype | analog +ganalog-dist.cfg +C'HOSTNAME IPv6_address_type_distribution' +O./distribution-ipv6addrtype/analog.html 
	# Create OUI statistics
	cat $LOGFILE | $BIN_IPV6LOGCON --out ouitype | analog +ganalog-dist.cfg +C'HOSTNAME OUI_type_distribution' +O./distribution-oui/analog.html 
	;;

    "combined")
	echo "Not supported by analog - exit"
	exit 1
	if [ ! -f $FILE_TMP ]; then
		# create combined file
		echo "Create combined logfile..."

		echo "addrtype..."
		cat $LOGFILE | $BIN_IPV6LOGCON --out addrtype >$FILE_TMP

		echo "ipv6addrtype..."
		cat $LOGFILE | $BIN_IPV6LOGCON --out ipv6addrtype >>$FILE_TMP
	
		echo "ouitype..."
		cat $LOGFILE | $BIN_IPV6LOGCON --out ouitype >>$FILE_TMP

		echo "...done"
	else
		echo "Combined logfile already exists"
	fi

	echo "Create statistics running 'analog'"
	cat $FILE_TMP | analog +ganalog-dist.cfg +C'HOSTNAME combined' +O./distribution-combined/analog.html 

	;;
    *)
	echo "Usage: $0 dedicated|combined"
esac
