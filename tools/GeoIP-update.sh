#!/bin/bash
#
# Simple shell script to update GeoIP database files
#
# Project    : ipv6calc/GeoIP
# File       : GeoIP-update.sh
# Version    : $Id: GeoIP-update.sh,v 1.1 2012/10/12 19:33:04 peter Exp $
# Copyright  : GNU GPL version 2
# Procuded   : 2012 by Peter Bieringer <pb (at) bieringer.de>


GEOIP_DAT_DIR="${GEOIP_DAT_DIR:-/var/local/share/GeoIP/}"

GEOIP_DAT_URL_BASE="http://geolite.maxmind.com/download/geoip/database/"

GEOIP_DAT_FILES="GeoLiteCountry/GeoIP.dat.gz GeoIPv6.dat.gz"

if [ ! -d "$GEOIP_DAT_DIR" ]; then
	echo "ERROR : missing directory: $GEOIP_DAT_DIR"
	exit 1
fi

if [ ! -w "$GEOIP_DAT_DIR" ]; then
	echo "ERROR : missing write permissions on directory: $GEOIP_DAT_DIR"
	exit 1
fi

# Download and unpack files
for file in $GEOIP_DAT_FILES; do
	file_dest="$GEOIP_DAT_DIR/`basename "$file"`"

	echo "INFO  : try to download file: $file ($file_dest)"
	wget -q -O "$file_dest" "$GEOIP_DAT_URL_BASE$file"
	if [ $? -ne 0 ]; then
		echo "ERROR : download of file not successful: $file ($file_dest)"
		continue
	fi
	echo "INFO  : download of file successful: $file ($file_dest)"

	gunzip -f "$file_dest"
	if [ $? -ne 0 ]; then
		echo "ERROR : unzip of file not successful: $file_dest"
		continue
	fi
	echo "INFO  : unzip of file successful: $file_dest"
done
