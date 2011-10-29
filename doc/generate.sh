#!/bin/sh

# $Id: generate.sh,v 1.4 2011/10/29 08:35:24 peter Exp $

# Requires: opensp

#export SP_ENCODING=UTF-8
export SP_CHARSET_FIXED=yes

if [ -z "$1" ]; then
	file_sgml="ipv6calc.sgml"
else
	file_sgml="$1"
fi

echo "Used SGML file: $file_sgml"

file_base="`basename $file_sgml .sgml`"

file_html="${file_base}.html"

file_dsl="./ldp.dsl"

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
	jade -t sgml -i html -V nochunks -d "${file_dsl}#html" $file_sgml >$file_html
	set +x
	local retval=$?
	if [ $retval -eq 0 ]; then
		echo "INF: Create HTML singlepage - done"
	else
		echo "ERR: Create HTML singlepage - an error occurs!"
	fi
	return $retval
}


### Main
validate_sgml || exit 1

create_html_singlepage || exit 1
