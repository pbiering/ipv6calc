/*
 * Project    : ipv6calc
 * File       : libifinet6.h
 * Version    : $Id$
 * Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libifinet6.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int libifinet6_ifinet6_to_ipv6addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv6addr *ipv6addrp);
extern int libifinet6_ifinet6_withprefixlength_to_ipv6addrstruct(const char *addrstring, const char *prefixlengthstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv6addr *ipv6addrp);
extern int libifinet6_ipv6addrstruct_to_ifinet6(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length);
