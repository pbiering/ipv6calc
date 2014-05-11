/*
 * Project    : ipv6calc
 * File       : librfc1884.h
 * Version    : $Id: librfc1884.h,v 1.3 2014/05/11 09:49:38 ds6peter Exp $
 * Copyright  : 2001-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for librfc1884.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int compaddr_to_uncompaddr(const char *addrstring, char *resultstring, const size_t resultstring_length);

extern int librfc1884_ipv6addrstruct_to_compaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);
extern int ipv6addrstruct_to_compaddr(const ipv6calc_ipv6addr *ipv6addr, char *resultstring, const size_t resultstring_length); /* going obsolete */
