#!/bin/sh

# $Id: test_showhelp.sh,v 1.1 2002/04/08 19:04:11 peter Exp $

echo "==Standard"
./ipv6calc -?

echo "==In"
./ipv6calc --in -?

echo "==Out"
./ipv6calc --out -?

echo "==Action"
./ipv6calc --action -?
