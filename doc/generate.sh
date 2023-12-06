#!/bin/sh
#
# generate HTML page out of SGML
#
# Project    : ipv6calc/doc
# File       : generate.sh
# Copyright  : 2003-2023 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL version 2
# Requires: openjade opens
# Optional: lyx docbook-utils
#
# 20231206/PB: change from "jade" to "openjade" (anyhow a softlink since at least EL6)
# 20231206/PB: add option to generate directly all from LyX file

#export SP_ENCODING=UTF-8
export SP_CHARSET_FIXED=yes

help() {
	cat <<END
$(basename "$0") [-L <file>] [<file>]
	-L <file>	Lyx file
	<file>		SGML file
END
}

# parse options
while getopts "h?L:" opt; do
    case $opt in
	L)
		file_lyx="$OPTARG"
		;;
	h|\?)
		help
		exit 0
		;;
	*)
		help
		exit 1
		;;
    esac
done

shift $[ $OPTIND - 1 ]

if [ -z "$1" ]; then
	file_sgml="ipv6calc.sgml"
else
	file_sgml="$1"
fi

echo "INF: Used SGML file: $file_sgml"

file_base="`basename $file_sgml .sgml`"

file_html="${file_base}.html"
file_xml="${file_base}.xml"

file_dsl="./ldp.dsl"

if [ -d /usr/share/sgml/docbook/ ]; then
	file_dsl_system=$(find /usr/share/sgml/docbook/ -type f -name ldp.dsl | tail -1 2>/dev/null)
fi

if [ -n "$file_dsl_system" ]; then
	file_dsl="$file_dsl_system"
	echo "INF: Used ldp.dsl file: $file_dsl (system-wide)"
else
	echo "INF: Used ldp.dsl file: $file_dsl (local)"
fi

if [ ! -f "$file_dsl" ]; then
	echo "ERR: Missing DSL file: $file_dsl"
	exit 1
fi

if [ ! -f $file_sgml ]; then
	echo "ERR: Missing SGML file, perhaps export DocBook of LyX won't work"
	exit 1
fi

validate_sgml() {
	echo "INF: Validate SGML code '$file_sgml'"
	set -x
	onsgmls -s $file_sgml
	local result=$?
	set +x
	if [ $result -gt 0 ]; then
		echo "ERR: Validation results in errors!"
		return 1
	else
		echo "INF: Validation was successfully"
	fi
}


create_html_singlepage() {
	echo "INF: Create HTML singlepage '$file_html'"
	set -x
	openjade -t sgml -i html -V nochunks -d "${file_dsl}#html" $file_sgml >$file_html
	local retval=$?
	set +x
	if [ $retval -eq 0 ]; then
		if head -1 $file_html | grep -q DOCTYPE; then
			echo "INF: Create HTML singlepage - done"
			return 0
		fi
	fi

	echo "ERR: Create HTML singlepage - an error occurs!"
	return $retval
}


export_sgml() {
	if [ ! -f "$file_lyx" ]; then
		echo "ERR: given Lyx file is not existing: $file_lyx"
		return 1
	fi

	if [ ! -r "$file_lyx" ]; then
		echo "ERR: given Lyx file is not readable: $file_lyx"
		return 1
	fi

	lyxBin=$(which "lyx" 2>/dev/null)
	if [ -z "$lyxBin" ]; then
		echo "ERR: no Lyx binary found"
		return 1
	fi

	echo "INF: Used Lyx file: $file_lyx"

	echo "INF: Export LyX->SGML"
	$lyxBin --export docbook $file_lyx || return 1

	echo "INF: Export LyX->XML"
	$lyxBin --export docbook-xml $file_lyx || return 1
}


### Main
if [ -n "$file_lyx" ]; then
	export_sgml || exit 1
fi

validate_sgml || exit 1

create_html_singlepage || exit 1
