/*
 * Project    : ipv6calc
 * File       : librfc2874.h
 * Version    : $Id: librfc2874.h,v 1.7 2014/05/11 09:49:38 ds6peter Exp $
 * Copyright  : 2001-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc2874.c
 */ 

#include "ipv6calc_inttypes.h"
#include "libipv6addr.h"


/* prototypes */
extern int librfc2874_addr_to_bitstring(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);
extern int librfc2874_bitstring_to_ipv6addrstruct(const char *inputstring, ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length);

extern int librfc2874_formatcheck(const char *string, char *infostring, const size_t infostring_length);
