/*
 * Project    : ipv6calc
 * File       : libifinet6.h
 * Version    : $Id: libifinet6.h,v 1.2 2002/03/02 19:02:27 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libifinet6.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int libifinet6_ifinet6_to_ipv6addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);
extern int libifinet6_ifinet6_withprefixlength_to_ipv6addrstruct(char *addrstring, char *prefixlengthstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);
