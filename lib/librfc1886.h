/*
 * Project    : ipv6calc
 * File       : librfc1886.h
 * Version    : $Id: librfc1886.h,v 1.2 2002/04/04 19:40:27 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc1886.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int librfc1886_addr_to_nibblestring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions, char* domain);
extern int librfc1886_nibblestring_to_ipv6addrstruct(const char *inputstring, ipv6calc_ipv6addr *ipv6addrp, char *resultstring);
extern int librfc1886_formatcheck(const char *string, char *infostring);
