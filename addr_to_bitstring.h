/*
 * Project    : ipv6calc
 * File       : addr_to_bitstring.h
 * Version    : $Id: addr_to_bitstring.h,v 1.3 2002/02/27 23:07:14 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  header file for addr_to_bitstring.c
 */

#include "libipv6addr.h"

/* prototypes */
extern void addr_to_bitstring_printhelp(void);
extern void addr_to_bitstring_printhelplong(void);
extern int  addr_to_bitstring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, long int command);
