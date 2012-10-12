#!/bin/bash
#
# Simple shell script to update IP2Location database files
#  requires credential file $HOME/.ip2location providing
#   login=EMAIL
#   password=PASSWORD
#
# Project    : ipv6calc/IP2Location
# File       : IP2Location-update.sh
# Version    : $Id: IP2Location-update.sh,v 1.1 2012/10/12 19:33:04 peter Exp $
# Copyright  : GNU GPL version 2
# Procuded   : 2012 by Peter Bieringer <pb (at) bieringer.de>


IP2LOCATION_DAT_DIR="${IP2LOCATION_DAT_DIR:-/var/local/share/IP2Location/}"

# source credentials (must provide login= and password=)
if [ -f "$HOME/.ip2location" ]; then
	source "$HOME/.ip2location"
else
	echo "ERROR : missing credential file: $HOME/.ip2location"
	exit 1
fi

for token in login password; do
	if [ -z "${!token}" ]; then
		echo "ERROR : credential file has not set: $token ($HOME/.ip2location)"
		exit 1
	fi
done

IP2LOCATION_DAT_URL_BASE="http://www.ip2location.com/download?login=$login&password=$password"

IP2LOCATION_DAT_FILES="IPV6BIN"

if [ ! -d "$IP2LOCATION_DAT_DIR" ]; then
	echo "ERROR : missing directory: $IP2LOCATION_DAT_DIR"
	exit 1
fi

if [ ! -w "$IP2LOCATION_DAT_DIR" ]; then
	echo "ERROR : missing write permissions on directory: $IP2LOCATION_DAT_DIR"
	exit 1
fi

# Download and unpack files
for file in $IP2LOCATION_DAT_FILES; do
	file_dest="$IP2LOCATION_DAT_DIR/`basename "$file".zip`"

	echo "INFO  : try to download file: $file ($file_dest)"
	wget -q -O "$file_dest" "$IP2LOCATION_DAT_URL_BASE&productcode=$file"
	if [ $? -ne 0 ]; then
		echo "ERROR : download of file not successful: $file ($file_dest)"
		continue
	fi
	echo "INFO  : download of file successful: $file ($file_dest)"

	if ! file "$file_dest" | grep -q "Zip archive data"; then
		echo "ERROR : downloaded file is not a ZIP archive: $file_dest"
		continue
	fi

	unzip -o -f -d "$IP2LOCATION_DAT_DIR" "$file_dest"
	if [ $? -ne 0 ]; then
		echo "ERROR : unzip of file not successful: $file_dest"
		continue
	fi
	echo "INFO  : unzip of file successful: $file_dest"
done
