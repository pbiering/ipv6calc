#!/bin/sh
#
# Simple shell script to create different types of statistics
#
# Example config file for analog creating some statistics
#
# Project    : ipv6calc/ipv6logconv
# File       : run_analog.sh
# Version    : $Id: run_analog.sh,v 1.1 2002/03/16 14:46:00 peter Exp $
# Copyright  : none
# Procuded   : 2002 by Peter Bieringer <pb (at) bieringer.de>

LOGFILE="../access_log"

BIN_IPV6LOGCON="../../ipv6logconv"

#set -x

# Create address type statistics
cat $LOGFILE | $BIN_IPV6LOGCON --out addrtype | analog +ganalog-dist.cfg +C'HOSTNAME Address_type_distribution' +O./distribution-addrtype/analog.html 

# Create IPv6 address type statistics
cat $LOGFILE | $BIN_IPV6LOGCON --out ipv6addrtype | analog +ganalog-dist.cfg +C'HOSTNAME IPv6_address_type_distribution' +O./distribution-ipv6addrtype/analog.html 

# Create OUI statistics
cat $LOGFILE | $BIN_IPV6LOGCON --out ouitype | analog +ganalog-dist.cfg +C'HOSTNAME OUI_type_distribution' +O./distribution-oui/analog.html 
