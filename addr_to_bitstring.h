/*
 * Project    : ipv6calc
 * File       : addr_to_bitstring.h
 * Version    : $Id: addr_to_bitstring.h,v 1.2 2002/02/25 21:18:50 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  header file for addr_to_bitstring.c
 */

/* prototypes */
extern void addr_to_bitstring_printhelp(void);
extern void addr_to_bitstring_printhelplong(void);
extern int  addr_to_bitstring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, long int command);
