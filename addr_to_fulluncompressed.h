/*
 * Project    : ipv6calc
 * File       : addr_to_fulluncompressed.h
 * Version    : $Id: addr_to_fulluncompressed.h,v 1.1 2002/02/25 21:18:50 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file addr_to_fulluncompressed.c
 */ 

/* prototypes */
extern int  addr_to_fulluncompressed(ipv6calc_ipv6addr *ipv6addr, char *resultstring, unsigned long command);
extern void addr_to_fulluncompressed_printhelp(void);
extern void addr_to_fulluncompressed_printhelplong(void);
