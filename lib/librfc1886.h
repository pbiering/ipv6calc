/*
 * Project    : ipv6calc
 * File       : librfc1886.h
 * Version    : $Id$
 * Copyright  : 2001-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc1886.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int librfc1886_addr_to_nibblestring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions, const char *domain);
extern int librfc1886_nibblestring_to_ipv6addrstruct(const char *inputstring, ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length);
extern int librfc1886_formatcheck(const char *string, char *infostring, const size_t infostring_length);
