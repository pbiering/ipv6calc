/*
 * Project    : ipv6calc
 * File       : librfc2874.h
 * Version    : $Id: librfc2874.h,v 1.4 2002/02/27 23:07:15 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc2874.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int librfc2874_addr_to_bitstring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, long int command);
