/*
 * Project    : ipv6calc
 * File       : addr_to_ip6int.h
 * Version    : $Id: addr_to_ip6int.h,v 1.2 2002/03/01 23:26:45 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for addr_to_ip6int.c
 */ 

#include "libipv6addr.h"

/* prototypes */
extern int  addr_to_ip6int(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, long int command);
extern void addr_to_ip6int_printhelp(void);
extern void addr_to_ip6int_printhelplong(void);
