/*
 * Project    : ipv6calc
 * File       : libifinet6.h
 * Version    : $Id: libifinet6.h,v 1.1 2002/03/01 23:27:25 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libifinet6.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int libifinet6_ifinet6_to_ipv6addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);
