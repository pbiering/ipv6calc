/*
 * Project    : ipv6calc
 * File       : librfc3056.h
 * Version    : $Id: librfc3056.h,v 1.1 2002/02/27 23:07:15 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc1884.c libary
 */ 

#include "libipv6addr.h"
#include "libipv4addr.h"

/* prototypes */
int ipv4addr_to_ipv6to4addr(ipv6calc_ipv6addr *ipv6addrp, ipv6calc_ipv4addr *ipv4addrp);
