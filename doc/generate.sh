#!/bin/sh

# $Id: generate.sh,v 1.1 2003/08/28 20:21:04 peter Exp $

if [ -z "$1" ]; then
	file_sgml="ipv6calc.sgml"
else
	file_sgml="$1"
fi

echo "Used SGML file: $file_sgml"

file_base="`basename $file_sgml .sgml`"

file_html="$file_base.html"

file_dsl="/usr/local/share/sgml/ldp.dsl"

if [ ! -f "$file_dsl" ]; then
	echo "ERR: Missing DSL file: $file_dsl"
	exit 1
fi


if [ ! -f $file_sgml ]; then
	echo "ERR: Missing SGML file, perhaps export DocBook of LyX won't work"
	exit 1
fi

# run sgmlfix
if [ -e ./runsgmlfix.sh ]; then
	./runsgmlfix.sh "$file_sgml"
else
	echo "WARN: cannot execute 'runsgmlfix.sh'"
fi

validate_sgml() {
	echo "INF: Validate SGML code '$file_sgml'"
	set -x
	nsgmls -s $file_sgml
	set +x
	if [ $? -gt 0 ]; then
		echo "ERR: Validation results in errors!"
		return 1
	else
		echo "INF: Validation was successfully"
	fi
}


create_html_singlepage() {
	echo "INF: Create HTML singlepage '$file_html'"
	set -x
	jade -t sgml -i html -V nochunks -d "/usr/local/share/sgml/ldp.dsl#html" $file_sgml >$file_html
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
validate_sgml
[ $? -ne 0 ] && exit 1

create_html_singlepage
[ $? -ne 0 ] && exit 1

