/*
 * Version:     $Id: librfc1884.h,v 1.1.1.1 2001/10/07 14:47:40 peter Exp $
 *
 * Header file for librfc1884.c libary
 *
 * Author:      Peter Bieringer <pb@bieringer.de>
 *
 */ 

/* prototypes */
#include "ipv6calc.h"

extern int compaddr_to_uncompaddr(char *addrstring, char *resultstring);
extern int ipv6addrstruct_to_compaddr(ipv6calc_ipv6addr *ipv6addr, char *resultstring);
