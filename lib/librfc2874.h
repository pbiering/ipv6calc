/*
 * Project    : ipv6calc
 * File       : librfc2874.h
 * Version    : $Id: librfc2874.h,v 1.6 2002/04/21 11:24:08 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc2874.c
 */ 

#include "ipv6calc_inttypes.h"
#include "libipv6addr.h"


/* prototypes */
extern int librfc2874_addr_to_bitstring(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions);
extern int librfc2874_bitstring_to_ipv6addrstruct(const char *inputstring, ipv6calc_ipv6addr *ipv6addrp, char *resultstring);

extern int librfc2874_formatcheck(const char *string, char *infostring);
