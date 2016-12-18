/*
 * Project    : ipv6calc
 * File       : librfc5569.h
 * Version    : $Id$
 * Copyright  : 2011 by Raphaël Assénat <raph (at) raphnet.net>
 *  Extensions: 2014-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc5569.c
 */ 

#include "libipv6addr.h"
#include "libipv4addr.h"

int librfc5569_calc_6rd_local_prefix(ipv6calc_ipv6addr *sixrd_prefix, const ipv6calc_ipv4addr *relay_prefix, const ipv6calc_ipv4addr *local_ip, char *resultstring, const size_t resultstring_length);
