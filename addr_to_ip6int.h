/*
 * Project    : ipv6calc
 * File       : addr_to_ip6int.h
 * Version    : $Id: addr_to_ip6int.h,v 1.1 2002/02/25 21:18:50 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for addr_to_ip6int.c
 */ 

/* prototypes */
extern int  addr_to_ip6int(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, long int command);
extern void addr_to_ip6int_printhelp(void);
extern void addr_to_ip6int_printhelplong(void);
