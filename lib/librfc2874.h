/*
 * Project    : ipv6calc
 * File       : librfc2874.h
 * Version    : $Id: librfc2874.h,v 1.3 2002/04/05 19:11:13 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc2874.c
 */ 

#if defined(__NetBSD__)
#include <inttypes.h>
#else
#include <stdint.h>
#endif

#include "libipv6addr.h"

/* prototypes */
extern int librfc2874_addr_to_bitstring(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions);
extern int librfc2874_bitstring_to_ipv6addrstruct(const char *inputstring, ipv6calc_ipv6addr *ipv6addrp, char *resultstring);

extern int librfc2874_formatcheck(const char *string, char *infostring);
