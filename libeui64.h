/*
 * Project    : ipv6calc
 * File       : libeui64.h
 * Version    : $Id: libeui64.h,v 1.2 2002/02/27 23:07:15 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libeu64.c
 */ 

#include "libipv6addr.h"
#include "libmac.h"

/* prototypes */
extern int create_eui64_from_mac(ipv6calc_ipv6addr *ipv6addrp, ipv6calc_macaddr *macaddrp);
