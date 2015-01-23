#!/bin/bash
#
# Simple shell script to update db-ip.com database files
#
# Project    : ipv6calc/DBIP
# File       : DBIP-update.sh
# Version    : $Id: DBIP-update.sh,v 1.3 2015/01/23 19:49:45 ds6peter Exp $
# Copyright  : 2014-2015 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL version 2


DBIP_DAT_DIR="${DBIP_DAT_DIR:-/var/local/share/DBIP/}"

DBIP_DAT_URL_BASE="http://download.db-ip.com/free/"

DBIP_DAT_FILES="dbip-country-%Y-%m.csv.gz dbip-city-%Y-%m.csv.gz"

DBIP_GENERATOR_LIST="./DBIP-generate-db.pl /usr/share/ipv6calc/tools/DBIP-generate-db.pl"

for entry in $DBIP_GENERATOR_LIST; do
	if [ -e "$entry" -a -x "$entry" ]; then
		generate_db="$entry"
		break
	fi
done

if [ -z "$generate_db" ]; then
	echo "ERROR : no DBIP database generator found from list: $DBIP_GENERATOR_LIST"
	exit 1
else
	echo "INFO  : selected DBIP database generator: $generate_db"
fi

if [ ! -t 0 ]; then
	options_generate="-q"
fi

if [ ! -d "$DBIP_DAT_DIR" ]; then
	echo "ERROR : missing directory: $DBIP_DAT_DIR"
	exit 1
fi

if [ ! -w "$DBIP_DAT_DIR" ]; then
	echo "ERROR : missing write permissions on directory: $DBIP_DAT_DIR"
	exit 1
fi

# Download files
download_result=1
if [ "$1" != "skip-download" ]; then
	for file in $DBIP_DAT_FILES; do
		# convert tokens
		year=$(date +%Y)
		month=$(date +%m)

		file=${file//%Y/$year}
		file=${file//%m/$month}

		file_dest="$DBIP_DAT_DIR/`basename "$file"`"

		echo "INFO  : try to download file: $file ($file_dest)"
		wget -q -O "$file_dest" "$DBIP_DAT_URL_BASE$file"
		if [ $? -ne 0 ]; then
			echo "ERROR : download of file not successful: $file ($file_dest)"
			continue
			download_result=0
		fi
		echo "INFO  : download of file successful: $file ($file_dest)"
	done
fi

# create db files from downloaded files
if [ "$download_result" = "1" ]; then
	for file in $DBIP_DAT_FILES; do
		# convert tokens
		year=$(date +%Y)
		month=$(date +%m)

		file=${file//%Y/$year}
		file=${file//%m/$month}

		file_input="$DBIP_DAT_DIR/`basename "$file"`"

		nice -n 19 $generate_db $options_generate -I "$file_input" -O "$DBIP_DAT_DIR"
	done
fi
