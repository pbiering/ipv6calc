/*
 * Project    : ipv6calc
 * File       : addr_to_ifinet6.h
 * Version    : $Id: addr_to_ifinet6.h,v 1.1 2002/02/25 21:18:50 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file addr_to_ifinet6.c
 */ 

/* prototypes */
extern int  addr_to_ifinet6(ipv6calc_ipv6addr *ipv6addrp, char *resultstring);
extern void addr_to_ifinet6_printhelp(void);
extern void addr_to_ifinet6_printhelplong(void);
