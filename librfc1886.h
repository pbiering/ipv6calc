/*
 * Project    : ipv6calc
 * File       : librfc1886.h
 * Version    : $Id: librfc1886.h,v 1.2 2002/03/02 17:27:28 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc1886.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int librfc1886_addr_to_nibblestring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, unsigned long int formatoptions, char* domain);
