/*
 * Project    : ipv6calc
 * File       : librfc6052.h
 * Version    : $Id$
 * Copyright  : 2015-2015 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc6052.c
 */ 

#include "libipv6addr.h"
#include "libipv4addr.h"

/* prototypes */
int librfc6052_ipv4addr_to_ipv6addr(ipv6calc_ipv6addr *ipv6addrp, const ipv6calc_ipv4addr *ipv4addrp);
int librfc6052_ipv6addr_to_ipv4addr(ipv6calc_ipv4addr *ipv4addrp, const ipv6calc_ipv6addr *ipv6addrp, char* resultstring, const size_t resultstring_length);
