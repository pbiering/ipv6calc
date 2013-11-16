#!/bin/bash
#
# Simple shell script to update IP2Location database files
#
# For updating non-sample files, credentials are required:
#  provided by file $HOME/.ip2location
#   login=EMAIL
#   password=PASSWORD
#
# Project    : ipv6calc/IP2Location
# File       : IP2Location-update.sh
# Version    : $Id: IP2Location-update.sh,v 1.3 2013/11/16 17:09:42 ds6peter Exp $
# Copyright  : 2012-2013 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL version 2

# Definitions:
IP2LOCATION_DAT_DIR="${IP2LOCATION_DAT_DIR:-/var/local/share/IP2Location/}"

IP2LOCATION_DAT_FILES="IPV6BIN"

IP2LOCATION_DAT_FILES_SAMPLE="20"


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
IP2LOCATION_DAT_URL_BASE_SAMPLE="http://www.ip2location.com/downloads"



if [ ! -d "$IP2LOCATION_DAT_DIR" ]; then
	echo "ERROR : missing directory: $IP2LOCATION_DAT_DIR"
	exit 1
fi

if [ ! -w "$IP2LOCATION_DAT_DIR" ]; then
	echo "ERROR : missing write permissions on directory: $IP2LOCATION_DAT_DIR"
	exit 1
fi

# Download and unpack non-sample files
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

	unzip -o -d "$IP2LOCATION_DAT_DIR" "$file_dest" '*.BIN'
	if [ $? -ne 0 ]; then
		echo "ERROR : unzip of file not successful: $file_dest"
		continue
	fi
	echo "INFO  : unzip of file successful: $file_dest"
done

# Download and unpack sample files
for number in $IP2LOCATION_DAT_FILES_SAMPLE; do
	file_dest="$IP2LOCATION_DAT_DIR/sample.bin.db$number.zip"

	echo "INFO  : try to download sample db: $number ($file_dest)"
	wget -q -O "$file_dest" "$IP2LOCATION_DAT_URL_BASE_SAMPLE/sample.bin.db$number.zip"
	if [ $? -ne 0 ]; then
		echo "ERROR : download of file not successful: DB$number ($file_dest)"
		continue
	fi
	echo "INFO  : download of file successful: DB$number ($file_dest)"

	if ! file "$file_dest" | grep -q "Zip archive data"; then
		echo "ERROR : downloaded file is not a ZIP archive: $file_dest"
		continue
	fi

	unzip -o -d "$IP2LOCATION_DAT_DIR" "$file_dest" '*.BIN'
	if [ $? -ne 0 ]; then
		echo "ERROR : unzip of file not successful: $file_dest"
		continue
	fi
	echo "INFO  : unzip of file successful: $file_dest"
done

# Adjust permissions
chmod 644 $IP2LOCATION_DAT_DIR/*.BIN
